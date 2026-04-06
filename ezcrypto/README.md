# `crypto` — Cryptography & Hashing for EZ

> Native-speed cryptography powered by `wincrypt.h` under the hood.
> Hashing, encoding, and AES-256 encryption in a clean, one-liner API.

---

## Overview

The `crypto` library gives EZ programs access to production-grade cryptographic primitives without any external dependencies. It wraps Windows' native `wincrypt.h` directly for maximum performance and security — no third-party C libraries, no overhead.

```ez
hashed = crypto.sha256("my secret data")
token  = crypto.base64Encode(hashed)
cipher = crypto.encrypt("sensitive info", "my-password")
```

---

## Installation

`crypto` is built into the EZ standard library. No imports needed — just use it:

```ez
digest = crypto.sha256("hello")
say digest
```

---

## API Reference

### Hashing

#### `crypto.sha256(text)` → `string`
Hashes the input using **SHA-256**. Returns a hex-encoded digest string.

```ez
say crypto.sha256("hello world")
# => b94d27b9934d3e08a52e52d7da7dabfac484efe04294e576f4b6b2d5f9edd490
```

---

#### `crypto.md5(text)` → `string`
Hashes the input using **MD5**. Returns a hex-encoded digest string.

> ⚠️ MD5 is not collision-resistant. Use for checksums and legacy compatibility only — not for security-sensitive hashing.

```ez
say crypto.md5("hello world")
# => 5eb63bbbe01eeed093cb22bb8f5acdc3
```

---

#### `crypto.sha1(text)` → `string`
Hashes the input using **SHA-1**. Returns a hex-encoded digest string.

> ⚠️ SHA-1 is deprecated for cryptographic use. Prefer SHA-256.

```ez
say crypto.sha1("hello world")
# => 2aae6c35c94fcfb415dbe95f408b9ce91ee846ed
```

---

#### `crypto.hash(algo, text)` → `string`
General-purpose hashing with an explicit algorithm name. Useful when the algorithm is chosen at runtime.

```ez
algo = "sha256"
say crypto.hash(algo, "hello world")
```

Supported values for `algo`:
- `"sha256"`
- `"sha1"`
- `"md5"`

---

### Base64

#### `crypto.base64Encode(text)` → `string`
Encodes any string or value to **Base64**.

```ez
encoded = crypto.base64Encode("hello:world")
say encoded   # => aGVsbG86d29ybGQ=
```

---

#### `crypto.base64Decode(text)` → `string`
Decodes a **Base64** string back to plaintext.

```ez
original = crypto.base64Decode("aGVsbG86d29ybGQ=")
say original   # => hello:world
```

---

### AES-256 Encryption

#### `crypto.encrypt(text, secret)` → `string`
Encrypts `text` using **AES-256** with the given `secret` password. Block padding and key derivation (PBKDF) are handled automatically.

Returns a Base64-encoded ciphertext string safe for storage or transmission.

```ez
cipher = crypto.encrypt("top secret message", "my-strong-password")
say cipher   # => U2FsdGVkX1...
```

---

#### `crypto.decrypt(cipherText, secret)` → `string`
Decrypts a ciphertext string produced by `crypto.encrypt`. Returns the original plaintext.

```ez
plain = crypto.decrypt(cipher, "my-strong-password")
say plain   # => top secret message
```

> **Note:** Decrypting with a wrong password will return an error or empty string — always validate output in sensitive contexts.

---

## Examples

### Storing a Hashed Password

```ez
password = input.getText()
hashed   = crypto.sha256(password)

# Store `hashed` in your database — never store the raw password
```

### Round-Trip Encryption

```ez
secret  = "hunter2"
message = "launch codes: 0000"

cipher  = crypto.encrypt(message, secret)
say "Encrypted: " + cipher

plain   = crypto.decrypt(cipher, secret)
say "Decrypted: " + plain
```

### Safe Token Generation from User Data

```ez
userId = "user_8821"
token  = crypto.base64Encode(crypto.sha256(userId))
say "Token: " + token
```

### Runtime Algorithm Selection

```ez
algos = ["sha256", "sha1", "md5"]

loop algo in algos {
    say algo + ": " + crypto.hash(algo, "test")
}
```

---

## Security Notes

| Feature | Detail |
|---------|--------|
| **AES key derivation** | Keys are derived from your password using a PBKDF structure — raw passwords are never used directly as AES keys |
| **Block padding** | Applied automatically — no manual padding required |
| **MD5 / SHA-1** | Provided for compatibility; avoid for new security-sensitive code |
| **Native backend** | Backed by `wincrypt.h` — no managed crypto, no third-party dependencies |

---

## Under the Hood

The `crypto` model is a thin EZ wrapper over native C functions:

| EZ call | Native function |
|---------|----------------|
| `crypto.hash(algo, text)` | `crypto_hash(algo, text)` |
| `crypto.base64Encode(text)` | `crypto_base64_encode(text)` |
| `crypto.base64Decode(text)` | `crypto_base64_decode(text)` |
| `crypto.encrypt(text, secret)` | `crypto_encrypt(text, secret)` |
| `crypto.decrypt(cipher, secret)` | `crypto_decrypt(cipher, secret)` |

All calls go through `wincrypt.h` APIs, keeping performance at the native layer and avoiding managed-code overhead.

---

## License

Part of the EZ standard library — MIT License. See [LICENSE](./LICENSE).

---

*Simple API. Native speed. No excuses for weak crypto.*
