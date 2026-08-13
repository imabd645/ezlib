# pki

Public-key signature verification for EZ. RSA and ECDSA, JWK and PEM keys,
X.509 certificate parsing.

```ez
use "pki"

key = PublicKey.fromJwk(jwksEntry)
key.verify("RS256", signingInput, signature)
```

## Install

```
ez install pki
```

Depends on `crypto`. Windows only — verification runs through CNG.

## Why

`crypto` covers the symmetric half: hashing, HMAC, AES. This is the other half —
verifying that something was signed by the holder of a private key you don't
have.

The gap it closes is concrete. `oauth` receives an OIDC `id_token` from Google,
Microsoft or Apple, and **every one of those is RS256**. Before this, `jwt`
could only check HMAC signatures, so the token could be decoded but not
verified — you were trusting the transport rather than the signature.

With `pki` installed, `jwt` verifies them properly:

```ez
use "jwt"
use "pki"

header = JWT.peek(token)["header"]              // read the kid, trust nothing
key = keyFromJwks(jwks, header["kid"])
result = JWT.verify(token, key, {
    "algorithms": ["RS256"],
    "issuer": "https://accounts.google.com",
    "audience": CLIENT_ID
})
```

## Keys

```ez
PublicKey.fromJwk({ "kty": "RSA", "n": …, "e": "AQAB" })
PublicKey.fromJwk({ "kty": "EC", "crv": "P-256", "x": …, "y": … })
PublicKey.fromPem(text)          // PUBLIC KEY, RSA PUBLIC KEY, or CERTIFICATE
PublicKey.fromRsa(modulus, exponent)
keyFromJwks(jwks, kid)
```

A JWKS carries several keys precisely so the issuer can rotate them, and the
token's header names the one that signed it. An unknown `kid` usually means the
issuer rotated and your cache is stale, so it says exactly that.

```ez
key.verify(algorithm, message, signature)   // -> bool
key.bits()      key.kind      key.fingerprint()
```

## Algorithms

`RS256` `RS384` `RS512` — RSASSA-PKCS1-v1_5
`PS256` `PS384` `PS512` — RSASSA-PSS
`ES256` `ES384` `ES512` — ECDSA on P-256, P-384, P-521

`none` is refused **by name**. Accepting it is the classic JWT forgery: strip
the signature, set `alg` to `none`, and everything verifies.

Using an algorithm from the wrong family — an RSA algorithm against an EC key —
throws rather than quietly returning false, because that is a programming
mistake and not a failed signature.

ECDSA signatures must be the raw `r||s` pair, which is what JWT carries. A
DER-wrapped signature (OpenSSL, X.509) needs `ecdsaDerToRaw` first; they are not
interchangeable.

## Certificates

```ez
cert = Certificate.fromPem(readFile("server.crt"))

cert.commonName()     cert.subject["O"]     cert.issuer["CN"]
cert.notBefore        cert.notAfter         cert.serial
cert.isValidAt(now)   cert.publicKey()
```

Expiry is the check people forget — a certificate is a statement with a
deadline on it.

This parses and reports; it does **not** validate a chain, check revocation, or
match a hostname. Those are separate jobs and pretending otherwise would be
worse than not offering them.

## Verification only

There is no signing here. Signing needs a private key, and a package that
accepts private key material invites people to put it somewhere it should not
be. Verification is also the operation that actually gates access.

The arithmetic runs through `bcrypt.dll`, not a hand-rolled bignum. Modular
exponentiation on 2048-bit integers written in an interpreter would be far too
slow per request, and it is exactly the kind of code that is subtly wrong in
ways a test suite does not catch.

## Tests

```
ez test.ez
```

62 assertions. Every key, signature and certificate was produced by .NET's
`System.Security.Cryptography`, not by this package — a verifier tested only
against its own output will happily accept signatures nobody else produces,
and might accept ones it should reject.

## License

MIT
