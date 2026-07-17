// Native crypto for the ezauth package.
//
// Password hashing is the one part of an auth library that is unforgiving: a
// plain SHA-256 of a password is cracked at billions of guesses/second on a GPU,
// so what is needed is a deliberately slow key-derivation function with a random
// per-password salt. That is PBKDF2-HMAC-SHA256 here, via Windows CNG (bcrypt.dll),
// because doing it in EZ would be both far too slow and far too easy to get wrong.
//
// Everything below is deliberately narrow: derive a key, HMAC a string, compare
// two strings in constant time. No parsing of untrusted structure beyond the
// encoded-hash format, which we produce ourselves.
//
// ABI NOTE: every function EZ calls with os_call(..., "int", ...) must return
// `long long`, never `int`. EZ types an "int" return as 64-bit and reads the
// whole register, but a 32-bit return only defines the low half -- so a negative
// result (our -1 "unreadable") comes back as 4294967295, which is positive and
// silently passes any `< 1` check. 0/1 returns appear to work by luck, which is
// what makes this worth stating rather than rediscovering.
#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#pragma comment(lib, "bcrypt.lib")

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

static const int SHA256_LEN = 32;
static const int SALT_LEN   = 16;

// ── helpers ─────────────────────────────────────────────────────────────────

static std::string to_hex(const unsigned char* p, size_t n) {
    static const char* d = "0123456789abcdef";
    std::string s;
    s.reserve(n * 2);
    for (size_t i = 0; i < n; ++i) { s += d[p[i] >> 4]; s += d[p[i] & 0xF]; }
    return s;
}

static bool from_hex(const std::string& h, std::vector<unsigned char>& out) {
    if (h.size() % 2) return false;
    out.clear();
    out.reserve(h.size() / 2);
    for (size_t i = 0; i < h.size(); i += 2) {
        auto nib = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        };
        int hi = nib(h[i]), lo = nib(h[i + 1]);
        if (hi < 0 || lo < 0) return false;
        out.push_back((unsigned char)((hi << 4) | lo));
    }
    return true;
}

// Compare without leaking WHERE the mismatch is. A normal strcmp/== returns as
// soon as it finds a differing byte, so its runtime reveals how many leading
// bytes were correct -- enough to reconstruct a digest or token one byte at a
// time. Always look at every byte and accumulate.
static bool const_time_eq(const unsigned char* a, const unsigned char* b, size_t n) {
    unsigned char diff = 0;
    for (size_t i = 0; i < n; ++i) diff |= (unsigned char)(a[i] ^ b[i]);
    return diff == 0;
}

static char* dup_out(const std::string& s) {
    char* out = new char[s.size() + 1];
    memcpy(out, s.c_str(), s.size() + 1);
    return out;
}

static bool random_bytes(unsigned char* buf, ULONG n) {
    // BCRYPT_USE_SYSTEM_PREFERRED_RNG = the OS CSPRNG. Never rand().
    return BCryptGenRandom(nullptr, buf, n, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == STATUS_SUCCESS;
}

// PBKDF2-HMAC-SHA256.
static bool pbkdf2(const unsigned char* pw, ULONG pwLen,
                   const unsigned char* salt, ULONG saltLen,
                   long long iterations, unsigned char* out, ULONG outLen) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    // BCRYPT_ALG_HANDLE_HMAC_FLAG: we want HMAC-SHA256 as the PRF, not raw SHA256.
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr,
                                    BCRYPT_ALG_HANDLE_HMAC_FLAG) != STATUS_SUCCESS) {
        return false;
    }
    NTSTATUS st = BCryptDeriveKeyPBKDF2(hAlg, (PUCHAR)pw, pwLen, (PUCHAR)salt, saltLen,
                                        (ULONGLONG)iterations, out, outLen, 0);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return st == STATUS_SUCCESS;
}

static bool hmac_sha256(const unsigned char* key, ULONG keyLen,
                        const unsigned char* msg, ULONG msgLen,
                        unsigned char* out) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr,
                                    BCRYPT_ALG_HANDLE_HMAC_FLAG) != STATUS_SUCCESS) {
        return false;
    }
    BCRYPT_HASH_HANDLE hHash = nullptr;
    NTSTATUS st = BCryptCreateHash(hAlg, &hHash, nullptr, 0, (PUCHAR)key, keyLen, 0);
    if (st != STATUS_SUCCESS) { BCryptCloseAlgorithmProvider(hAlg, 0); return false; }

    st = BCryptHashData(hHash, (PUCHAR)msg, msgLen, 0);
    if (st == STATUS_SUCCESS) st = BCryptFinishHash(hHash, out, SHA256_LEN, 0);

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return st == STATUS_SUCCESS;
}

// Split "a$b$c$d" on '$'.
static std::vector<std::string> split_dollar(const std::string& s) {
    std::vector<std::string> parts;
    size_t start = 0;
    for (;;) {
        size_t p = s.find('$', start);
        if (p == std::string::npos) { parts.push_back(s.substr(start)); break; }
        parts.push_back(s.substr(start, p - start));
        start = p + 1;
    }
    return parts;
}

extern "C" {

// Hash a password. Returns "pbkdf2_sha256$<iterations>$<salt_hex>$<hash_hex>"
// (the same shape Django uses: self-describing, so the cost can be raised later
// without invalidating existing hashes). Salt is fresh per call, so identical
// passwords never produce identical hashes. Caller frees with auth_free.
// Returns nullptr on failure -- never a fallback hash.
__declspec(dllexport) char* auth_hash_password(const char* password, long long iterations) {
    if (!password || iterations < 1) return nullptr;

    unsigned char salt[SALT_LEN];
    if (!random_bytes(salt, SALT_LEN)) return nullptr;

    unsigned char key[SHA256_LEN];
    if (!pbkdf2((const unsigned char*)password, (ULONG)strlen(password),
                salt, SALT_LEN, iterations, key, SHA256_LEN)) {
        return nullptr;
    }

    std::string enc = "pbkdf2_sha256$" + std::to_string(iterations) + "$" +
                      to_hex(salt, SALT_LEN) + "$" + to_hex(key, SHA256_LEN);
    return dup_out(enc);
}

// Verify a password against an encoded hash. Returns 1 on match, 0 on mismatch
// or malformed input. The comparison is constant-time.
__declspec(dllexport) long long auth_verify_password(const char* password, const char* encoded) {
    if (!password || !encoded) return 0;

    std::vector<std::string> p = split_dollar(encoded);
    if (p.size() != 4 || p[0] != "pbkdf2_sha256") return 0;

    long long iters = _strtoi64(p[1].c_str(), nullptr, 10);
    if (iters < 1) return 0;

    std::vector<unsigned char> salt, expect;
    if (!from_hex(p[2], salt) || !from_hex(p[3], expect)) return 0;
    if (expect.size() != SHA256_LEN || salt.empty()) return 0;

    unsigned char actual[SHA256_LEN];
    if (!pbkdf2((const unsigned char*)password, (ULONG)strlen(password),
                salt.data(), (ULONG)salt.size(), iters, actual, SHA256_LEN)) {
        return 0;
    }
    return const_time_eq(actual, expect.data(), SHA256_LEN) ? 1 : 0;
}

// The iteration count baked into an encoded hash, or -1 if unreadable. Lets EZ
// decide whether a stored hash was made with a now-too-low cost and should be
// upgraded on the user's next successful login.
__declspec(dllexport) long long auth_hash_iterations(const char* encoded) {
    if (!encoded) return -1;
    std::vector<std::string> p = split_dollar(encoded);
    if (p.size() != 4 || p[0] != "pbkdf2_sha256") return -1;
    long long it = _strtoi64(p[1].c_str(), nullptr, 10);
    if (it < 1) return -1;
    return it;
}

// HMAC-SHA256(key, msg) as lowercase hex. Used to sign values that travel
// through the client (CSRF tokens, remember-me cookies) so tampering is
// detectable without keeping server-side state. Caller frees with auth_free.
__declspec(dllexport) char* auth_hmac_sha256(const char* key, const char* msg) {
    if (!key || !msg) return nullptr;
    unsigned char out[SHA256_LEN];
    if (!hmac_sha256((const unsigned char*)key, (ULONG)strlen(key),
                     (const unsigned char*)msg, (ULONG)strlen(msg), out)) {
        return nullptr;
    }
    return dup_out(to_hex(out, SHA256_LEN));
}

// Constant-time string equality, exposed for EZ. `==` on two strings in EZ
// short-circuits on the first differing byte, which leaks a prefix-match length
// to anyone who can time it -- do not use it to compare tokens or digests.
__declspec(dllexport) long long auth_const_time_eq(const char* a, const char* b) {
    if (!a || !b) return 0;
    size_t la = strlen(a), lb = strlen(b);
    // Length is not a secret here (our tokens are fixed-length), but the content
    // is: bail on length first, then compare every byte of equal-length input.
    if (la != lb) return 0;
    return const_time_eq((const unsigned char*)a, (const unsigned char*)b, la) ? 1 : 0;
}

__declspec(dllexport) void auth_free(char* p) {
    if (p) delete[] p;
}

} // extern "C"
