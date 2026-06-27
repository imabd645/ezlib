# crypto — EZ Cryptography Library (v2.0)

A comprehensive cryptography library for EZ built on Windows CryptoAPI (Advapi32) and BCrypt. Provides real AES encryption, SHA hashing, HMAC, PBKDF2 key derivation, cryptographically secure random, UUID generation, and Base64 encoding — all through a clean, simple API.

---

## Installation

```ez
use "crypto"
```

Requires Windows with `Advapi32.dll` and `bcrypt.dll` — both ship with every modern Windows installation.

---

## Quick Start

```ez
use "crypto"

# Hash a password
hash = Crypto.sha256("hello world")
out hash  # → 2cf24dba5fb0a30e...

# Encrypt with AES-256
key = "12345678901234567890123456789012"  # 32 bytes
iv  = "1234567890123456"                  # 16 bytes
encrypted = Crypto.aesEncrypt(key, iv, "secret message")
decrypted = Crypto.aesDecrypt(key, iv, encrypted)
out decrypted  # → secret message

# Generate a UUID
id = Crypto.uuid()
out id  # → 550e8400-e29b-41d4-a716-446655440000
```

---

## API Reference

### `Crypto` — Unified Facade

The easiest way to use ezcrypto. A single model exposing the most common operations.

```ez
use "crypto"
```

#### Hashing

```ez
Crypto.sha256(text)   # → lowercase hex string (64 chars)
Crypto.sha1(text)     # → lowercase hex string (40 chars)
Crypto.md5(text)      # → lowercase hex string (32 chars)
```

```ez
out Crypto.sha256("hello")
# → 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
```

#### AES Encryption (AES-256-CBC)

```ez
Crypto.aesEncrypt(key, iv, plaintext)  # → Base64 string
Crypto.aesDecrypt(key, iv, ciphertext) # → plaintext string
```

| Parameter | Type | Description |
|---|---|---|
| `key` | string | 16, 24, or 32 characters (AES-128/192/256) |
| `iv` | string | Exactly 16 characters (AES block size) |
| `plaintext` | string | Text to encrypt |
| `ciphertext` | string | Base64 string from `aesEncrypt` |

```ez
key = "mySecretKey12345"          # 16 bytes → AES-128
iv  = "myInitialVector!"          # 16 bytes
encrypted = Crypto.aesEncrypt(key, iv, "Hello EZ!")
decrypted = Crypto.aesDecrypt(key, iv, encrypted)
```

#### HMAC

```ez
Crypto.hmac(key, message)  # → HMAC-SHA256 hex string
```

```ez
mac = Crypto.hmac("secret-key", "important message")
```

#### UUID v4

```ez
Crypto.uuid()  # → RFC 4122 UUID v4 string
```

```ez
id = Crypto.uuid()
out id  # → 550e8400-e29b-41d4-a716-446655440000
```

#### Base64

```ez
Crypto.base64Encode(text)  # → Base64 string
Crypto.base64Decode(text)  # → original string
```

---

### `Hash` — Direct Hashing

Lower-level hashing with both hex string and raw byte array output.

```ez
Hash.sha256(text)       # → hex string
Hash.sha1(text)         # → hex string
Hash.md5(text)          # → hex string

Hash.sha256Bytes(text)  # → array of ints [0..255]
Hash.sha1Bytes(text)    # → array of ints [0..255]
Hash.md5Bytes(text)     # → array of ints [0..255]
```

```ez
use "ezcrypto"

# Get raw bytes for further processing
bytes = Hash.sha256Bytes("hello")
out len(bytes)  # → 32
```

---

### `AES` — AES Encryption (CBC Mode)

AES-128, AES-192, and AES-256 in CBC mode via Windows CryptoAPI. Key length determines the variant automatically.

```ez
AES.encrypt(key, iv, plaintext)   # → Base64 ciphertext
AES.decrypt(key, iv, ciphertext)  # → plaintext string
```

| Key Length | AES Variant |
|---|---|
| 16 bytes | AES-128 |
| 24 bytes | AES-192 |
| 32 bytes | AES-256 |

```ez
use "crypto"

key = "12345678901234567890123456789012"  # 32 bytes = AES-256
iv  = "1234567890123456"                  # always 16 bytes

cipher = AES.encrypt(key, iv, "classified information")
plain  = AES.decrypt(key, iv, cipher)
out plain  # → classified information
```

> **Security note:** Never reuse the same IV with the same key. Generate a fresh IV with `SecureRandom.bytes(16)` for each encryption operation and store it alongside the ciphertext.

---

### `HMAC` — Hash-Based Message Authentication

RFC 2104 compliant HMAC using SHA-256, SHA-1, or MD5. Pure EZ implementation built on top of `Hash`.

```ez
HMAC.sha256(key, message)  # → hex string
HMAC.sha1(key, message)    # → hex string
HMAC.md5(key, message)     # → hex string
```

```ez
use "crypto"

mac = HMAC.sha256("my-secret-key", "message to authenticate")
out mac
```

---

### `PBKDF2` — Password Key Derivation

RFC 2898 compliant PBKDF2 using HMAC-SHA256. Use this to derive encryption keys from passwords — never use a raw password directly as an AES key.

```ez
PBKDF2.derive(password, salt, iterations, keyLen)
# → lowercase hex string of keyLen bytes
```

| Parameter | Default | Description |
|---|---|---|
| `password` | required | User's password |
| `salt` | required | Random salt (store alongside hash) |
| `iterations` | 100000 | Higher = slower = more secure |
| `keyLen` | 32 | Output key length in bytes |

```ez
use "crypto"

# Derive a 32-byte AES-256 key from a password
salt   = "random-salt-value"
keyHex = PBKDF2.derive("user-password", salt, 100000, 32)
out keyHex  # → 64 character hex string (32 bytes)
```

> **Recommendation:** Use at least 100,000 iterations. Generate the salt with `SecureRandom.bytes(16)`.

---

### `SecureRandom` — Cryptographically Secure Randomness

Backed by `BCryptGenRandom` — the Windows cryptographically secure RNG. Suitable for key generation, tokens, nonces, and IVs.

```ez
SecureRandom.bytes(n)          # → array of n random ints [0..255]
SecureRandom.randInt(min, max) # → random int in [min, max]
SecureRandom.randFloat()       # → random float in [0.0, 1.0)
SecureRandom.uuid4()           # → UUID v4 string
```

```ez
use "crypto"

# Generate a random 16-byte IV for AES
ivBytes = SecureRandom.bytes(16)

# Random number between 1 and 100
n = SecureRandom.randInt(1, 100)

# UUID
id = SecureRandom.uuid4()
```

---

### `Base64` — Base64 Encoding and Decoding

Accepts both strings and byte arrays as input.

```ez
Base64.encode(input)         # string or byte array → Base64 string
Base64.decode(encoded)       # Base64 string → original string
Base64.decodeToBytes(encoded) # Base64 string → byte array [0..255]
```

```ez
use "crypto"

encoded = Base64.encode("Hello, EZ!")
out encoded  # → SGVsbG8sIEVaIQ==

decoded = Base64.decode(encoded)
out decoded  # → Hello, EZ!
```

---

### `RC4` — ⚠️ Deprecated

RC4 is a broken stream cipher deprecated since 2015 (RFC 7465). It is kept only for backwards compatibility with existing data. **Do not use RC4 for new code.**

```ez
# DO NOT USE FOR NEW CODE
cipher = RC4("key")
encrypted = cipher.processBytes("plaintext")  # returns byte array
```

Using `Crypto.rc4Encrypt` or `Crypto.rc4Decrypt` will print a deprecation warning automatically. Migrate to `AES` immediately.

---

## Common Patterns

### Password Hashing

```ez
use "crypto"

# Store this salt and hash in your database
salt     = "random-unique-salt"
password = "user-password"

stored = PBKDF2.derive(password, salt, 100000, 32)

# Verify later
verify = PBKDF2.derive("user-password", salt, 100000, 32)
out stored == verify  # → true
```

### Secure File Encryption

```ez
use "crypto"

# Derive key from password
salt   = "my-app-salt"
keyHex = PBKDF2.derive("password", salt, 100000, 32)

# Use first 32 chars of hex as key (32 ASCII chars = 32 bytes)
key = substr(keyHex, 0, 32)

# Generate random IV
ivBytes = SecureRandom.bytes(16)
iv = ""
get b in ivBytes { iv = iv + chr(b) }

# Encrypt
content   = readFile("secret.txt")
encrypted = AES.encrypt(key, iv, content)
writeFile("secret.enc", encrypted)
```

### API Request Signing

```ez
use "crypto"

task signRequest(secretKey, method, path, body) {
    timestamp = str(clock())
    payload   = method + "\n" + path + "\n" + timestamp + "\n" + body
    signature = HMAC.sha256(secretKey, payload)
    give { sig: signature, ts: timestamp }
}

result = signRequest("my-secret", "POST", "/api/data", "{}")
out result["sig"]
```

---

## Security Notes

- **AES key length:** 32 bytes (AES-256) is recommended for new code
- **IV generation:** Always generate a fresh random IV per encryption — never hardcode or reuse IVs
- **IV storage:** Store the IV alongside the ciphertext — it is not secret, just must be unique
- **Password hashing:** Always use `PBKDF2` to derive keys from passwords, never use raw passwords as keys
- **Iterations:** PBKDF2 with 100,000+ iterations is the minimum recommendation (NIST 2023)
- **RC4:** Do not use under any circumstances for new code

---

## Requirements

- Windows (uses `Advapi32.dll` and `bcrypt.dll`)
- EZ runtime with FFI support (`os_load_lib`, `os_call`)
- Both DLLs ship with Windows Vista and later — no additional installation needed

---

## Changelog

### v2.0
- Added real **AES-128/192/256-CBC** via Windows CryptoAPI — replaces the false AES claim in v1
- Added **HMAC-SHA256/SHA1/MD5** (pure EZ, RFC 2104)
- Added **PBKDF2** key derivation (pure EZ, RFC 2898)
- Added **SecureRandom** backed by `BCryptGenRandom`
- Added **UUID v4** generation
- **RC4 deprecated** — prints runtime warning, kept for compatibility only
- SHA-256, SHA-1, MD5 moved to real CryptoAPI implementation

### v1.0
- Base64 encode/decode
- RC4 (now deprecated)
- SHA-256 labeled incorrectly as AES (fixed in v2.0)
