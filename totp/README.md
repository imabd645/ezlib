# totp — two-factor codes for EZ

> **Import:** `use "totp"`
> **Install:** `ez install totp`
> **Depends on:** `crypto`

Time-based one-time passwords (RFC 6238) and their counter-based counterpart
(RFC 4226) — the six-digit codes an authenticator app shows.

```ez
use "totp"

secret = TOTP.generateSecret()
when TOTP.verify(secret, submittedCode) { signIn() }
```

Verified against every published test vector in both RFCs.

---

## Enrolling a user

```ez
use "totp"
use "qr"

# Once, when the user turns on 2FA:
secret = TOTP.generateSecret()
saveSecretFor(user, secret)          # store it as carefully as a password

uri = TOTP.uri(secret, user.email, "My App")
qrImage = QRCode(uri, 0)             # the user scans this

# Show the secret too, for anyone entering it by hand:
out TOTP.formatSecret(secret)        # JBSW Y3DP EHPK 3PXP
```

## Verifying

```ez
when TOTP.verify(secret, code) { grantAccess() }
other { rejectWithMessage() }
```

`verify` accepts the current code plus **one step either side**, because
clocks drift and because a user may type a code as it rolls over. Widen it
with `verifyWith` if you must, knowing a stolen code stays usable longer.

## API

| Call | Result |
| --- | --- |
| `TOTP.generate(secret)` | The code for right now |
| `TOTP.at(secret, seconds)` | The code for a moment; nil means now |
| `TOTP.verify(secret, code)` | Check with a one-step window |
| `TOTP.verifyWith(secret, code, window, period, digits, algorithm)` | Full control |
| `TOTP.atWith(secret, seconds, period, digits, algorithm)` | Full control |
| `TOTP.hotp(secret, counter, digits, algorithm)` | Counter-based (RFC 4226) |
| `TOTP.secondsRemaining()` | Until the current code expires, for a countdown |
| `TOTP.generateSecret()` | A new 20-byte secret, base32 |
| `TOTP.generateSecretOf(bytes)` | A secret of a given length |
| `TOTP.isValidSecret(secret)` | Whether it decodes |
| `TOTP.formatSecret(secret)` | Grouped in fours for manual entry |
| `TOTP.uri(secret, account, issuer)` | `otpauth://` URI for a QR code |
| `TOTP.constantTimeEquals(a, b)` | Timing-safe comparison |

Defaults are 30-second periods, 6 digits and SHA-1 — what authenticator apps
expect. `SHA256` and `SHA512` are supported but many apps ignore the
algorithm parameter and assume SHA-1, so changing it can silently break
enrolment.

## Security notes

**The secret is a credential.** Anyone holding it can generate valid codes
forever. Store it with the same care as a password hash, and show it to the
user exactly once.

**Comparison is constant-time.** A comparison that returns early on the first
wrong character leaks, through timing, how much of a guess was right — which
is enough to find a code digit by digit. `TOTP.verify` never short-circuits.

**Rate-limit verification anyway.** Six digits is a million possibilities, and
a window of three steps triples the acceptable answers. Without a limit on
attempts, that is brute-forceable.

**Replay is not prevented here.** A code stays valid for its whole period, so
a captured one can be used again within that window. If that matters, record
the last counter you accepted per user and refuse to reuse it.

## About the implementation

A TOTP code is an HMAC over a counter that advances every 30 seconds,
truncated to the last digits. Both the key and the counter are **binary** —
and EZ strings are UTF-8, so building them with `chr()` would turn any byte
above 127 into two and produce a wrong digest. They are assembled through a
raw buffer instead, which is byte-exact.

That is also why the RFC vectors matter: they are the difference between
"self-consistent" and "correct". All of them pass:

| T | Expected | |
| --- | --- | --- |
| 59 | 94287082 | ✓ |
| 1111111109 | 07081804 | ✓ |
| 1111111111 | 14050471 | ✓ |
| 1234567890 | 89005924 | ✓ |
| 2000000000 | 69279037 | ✓ |
| 20000000000 | 65353130 | ✓ |

Plus RFC 4226 counters 0–5 for HOTP, and RFC 4648 vectors for base32.

## Testing

```
ez test.ez
```

65 tests: the RFC vectors above, base32 round-trips, digit padding, window
behaviour, constant-time comparison, secret generation and URI construction.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `TOTP` model: codes, verification, secrets, enrolment URIs |
| `base32.ez` | RFC 4648 base32, tolerant of spacing, padding and case |

## License

MIT
