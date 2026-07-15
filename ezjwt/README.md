# jwt — JSON Web Tokens for EZ

> **Version:** 1.0  
> **Import:** `use "jwt"`  
> **File:** `C:\ezlib\jwt\main.ez`

---

## Overview

The `jwt` library provides encoding and decoding of JSON Web Tokens (JWT) using the industry-standard **HS256** (HMAC-SHA256) algorithm. It relies on the native cryptographic functions provided by `ezcrypto`.

---

## Quick Start

```ez
use "jwt"

SECRET = "my-super-secret-key-123"

# 1. Create a payload dictionary
payload = {
    "sub": "user_42",
    "role": "admin",
    "exp": clock() + 3600000  # Expires in 1 hour
}

# 2. Encode to a JWT string
token = JWT.encode(payload, SECRET)
out "Generated Token: " + token

# 3. Decode and verify the JWT string
try {
    decodedPayload = JWT.decode(token, SECRET)
    out "Decoded Subject: " + decodedPayload["sub"]
} catch e {
    out "Verification failed: " + str(e)
}
```

---

## Model: `JWT`

### `JWT.encode(payload, secret)` → `string`

Generates an HS256 JWT string.

**Parameters:**
- `payload` — A dictionary containing the claims to embed in the token.
- `secret` — The secret key string used to sign the token.

**Returns:** A standard `header.payload.signature` JWT string encoded in Base64Url format.

---

### `JWT.decode(token, secret)` → `dict`

Decodes a JWT and verifies its HS256 signature.

**Parameters:**
- `token` — The JWT string to verify.
- `secret` — The secret key string that was used to sign the token.

**Returns:** The decoded payload dictionary.

**Throws:**
- `"jwt: Invalid token format"` if the token does not have 3 parts.
- `"jwt: Signature verification failed"` if the signature does not match or the token was tampered with.
- `"jwt: Unsupported algorithm"` if the header does not specify `HS256`.

---

## Notes
- This library uses **Base64Url** encoding (as required by the JWT RFC 7519), which makes the generated tokens safe to use in URLs without escaping.
- It only supports symmetric `HS256`. Asymmetric algorithms (like RS256) are not currently supported by `ezcrypto`.
