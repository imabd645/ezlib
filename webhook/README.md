# webhook

Verifying that an inbound webhook really came from who it claims.

```ez
use "webhook"

ok = Stripe(SIGNING_SECRET).verify(rawBody, req["headers"])
when ok == false { give { "status": 400 } }
```

## Install

```
ez install webhook
```

Depends on `crypto`.

## Why

An endpoint that accepts webhooks without checking the signature accepts them
from **anyone**. The URL is the only secret, and URLs leak — into logs, proxies,
browser history, screenshots, error reports. "Mark this invoice paid" is one
POST away.

## Providers

| | scheme |
|---|---|
| `Stripe(secret)` | HMAC-SHA256 over `{timestamp}.{body}`, hex, in a compound header |
| `GitHub(secret)` | HMAC-SHA256 over the raw body, hex, `sha256=` prefixed |
| `Slack(secret)` | HMAC-SHA256 over `v0:{timestamp}:{body}`, hex, `v0=` prefixed |
| `Shopify(secret)` | HMAC-SHA256 over the raw body, **base64** not hex |
| `Twilio(authToken)` | HMAC-SHA1 over the URL with its sorted POST fields appended |
| `Generic(secret, alg, encoding)` | anything else that HMACs a body |

```ez
Stripe(secret).verify(body, headers)
GitHub(secret).verify(body, headers)
Slack(secret).verify(body, headers)
Shopify(secret).verify(body, headers)
Twilio(token).verify(url, params, headers)
```

`verify` gives you a boolean. When you want to know *why* it failed — for a log
line, never for the response — use `check`, which returns a `Result`:

```ez
result = Stripe(secret).check(body, header)
result.ok        // false
result.reason    // "the timestamp is 412s outside the 300s tolerance …"
```

Header lookup is case-insensitive, and a repeated header takes the first value.

## Two rules that catch everyone

**The signature covers the raw body, byte for byte.** Parse the JSON and
re-serialise it and the bytes change — key order, spacing, unicode escapes — and
every signature fails. Keep the body exactly as received and verify *before*
parsing. This is the single commonest cause of "my webhook verification doesn't
work".

**The comparison must be constant-time.** A normal `==` stops at the first
differing byte, so how long it takes reveals how many leading bytes were right.
Against an endpoint an attacker can call repeatedly, that recovers a valid
signature one byte at a time. `constantTimeEquals` examines every byte.

## Replay protection

A signature stays valid forever — it's just an HMAC of bytes that don't change.
The only thing stopping a captured request being sent again is the timestamp.
Stripe and Slack sign one, and both are checked against a tolerance:

```ez
Stripe(secret, 600)          // widen from the default 300 seconds
```

The window cuts both ways, since clock skew makes a slightly *future* timestamp
normal.

## Notes per provider

- **Stripe** may send several `v1=` signatures in one header during a secret
  rotation. Any one matching is a pass — taking only the first breaks every
  rotation.
- **GitHub** still sends the SHA-1 `X-Hub-Signature`. It is deliberately
  refused: accepting it lets an attacker choose the weaker algorithm.
- **Shopify** is base64, not hex. A verifier copied from the GitHub one silently
  rejects everything.
- **Twilio** signs the URL, not the body. Getting the URL wrong — `http` vs
  `https`, a missing port, a proxy rewriting the host — is the usual reason this
  fails in production, so the rejection message says so.

Malformed input is rejected, never fatal. A forged header carrying `t=abc`
returns a rejection rather than throwing — a bad webhook should not be able to
take down the handler.

## Tests

```
ez test.ez
```

73 assertions. The signatures were produced by an independent implementation
(.NET's `HMACSHA256`/`HMACSHA1`) over the same inputs, not read back out of this
code — a verifier tested against its own output will happily accept a scheme no
real provider uses.

## License

MIT
