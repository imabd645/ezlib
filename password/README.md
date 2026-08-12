# password

Password hashing for EZ. PBKDF2-HMAC-SHA256 with per-hash salts and parameters,
timing-safe verification, and rehash detection.

```ez
use "password"

stored = hashPassword(plaintext)               # store this
ok = verifyPassword(attempt, stored)           # check it later
```

That is the whole API for the common case. Everything else is for the cases
that come up once the first version is in production.

## Install

```
ez install password
```

## Why not just hash it

A password put through SHA-256 is a password you have not protected. SHA-256 is
built to be fast, which is exactly the wrong property: a modern GPU tries
billions of candidates a second against it, and the top of any leaked password
list falls in seconds.

PBKDF2 is the same hash repeated hundreds of thousands of times. That costs you
a fraction of a second per login and costs an attacker the same factor per
guess, which is the entire point.

The salt is what stops two identical passwords sharing a hash. Without it, one
cracked password exposes every account that chose it, and precomputed tables
work.

## The stored format

```
pbkdf2-sha256$600000$3f9a…$8b1c…
 algorithm     cost   salt  hash
```

The parameters travel with the hash. That matters more than it sounds: when you
raise the cost factor next year, every existing hash still verifies against the
count it was made with. Storing the cost separately, or assuming the current
default, logs everybody out at once.

## Raising the cost

```ez
when Password.needsRehash(stored) {
    stored = Password.hash(plaintext)          # you have the plaintext here
    saveUser(user)
}
```

The only moment you legitimately hold a plaintext password is during a
successful login. That is where the upgrade goes.

`needsRehash` is also `true` for a hash from an algorithm this version no longer
recognises, and for anything malformed.

## Speed, and the native path

The default is 600,000 iterations — the current OWASP recommendation for
PBKDF2-HMAC-SHA256.

Doing that in EZ is not viable: `crypto`'s PBKDF2 crosses the FFI boundary
twice per iteration, at roughly 1.5 ms each, which puts a properly-sized hash at
about fifteen minutes. So this package calls `BCryptDeriveKeyPBKDF2`, which does
the whole derivation inside one call:

| | 600,000 iterations |
|---|---|
| native | ~165 ms |
| pure EZ | ~15 minutes |

```ez
Password.isNative()                            # true where the OS provides it
Password.defaultIterations()                   # 600000, or 20000 without it
```

Where the native path is unavailable the iteration count drops to a value that
still runs, and the count is recorded in the hash as always, so those hashes
keep verifying after a machine gains it. A password library that refuses to run
is worse than a slower one.

Both paths produce identical output; the test suite checks them against each
other and against a published PBKDF2-HMAC-SHA256 vector.

## Verification is timing-safe

```ez
constantTimeEquals(a, b)
```

Comparison never returns early on the first differing byte. An early return
leaks how much of a candidate was right, which is enough to recover a hash one
character at a time over enough attempts.

Malformed input fails the login rather than crashing the request — a garbage
value in the password column should log someone out, not return a 500.

## Strength

```ez
result = Password.strength(candidate)
# { ok, score, problems }
```

Length first, because it dominates: a long passphrase beats a short one with a
symbol in it. Also rejects the common list and single repeated characters.

This is advice for the user at signup, not a gate on the hash. Refusing to
*store* an unusual password is how people end up writing them down.

## API

| call | does |
|---|---|
| `hashPassword(plaintext)` | hash at the default cost |
| `verifyPassword(plaintext, stored)` | `true` or `false`, never a throw for bad input |
| `Password.hash(plaintext)` | the same as `hashPassword` |
| `Password.hashWith(plaintext, iterations)` | choose the cost |
| `Password.verify(plaintext, stored)` | the same as `verifyPassword` |
| `Password.needsRehash(stored)` | `true` if it is below current defaults |
| `Password.isValidHash(stored)` | shape check without verifying |
| `Password.describe(stored)` | `{ algorithm, iterations, saltBytes }`, or `nil` |
| `Password.strength(plaintext)` | `{ ok, score, problems }` |
| `Password.isNative()` | whether the OS is doing the derivation |
| `Password.defaultIterations()` | the cost `hash` will use |
| `constantTimeEquals(a, b)` | timing-safe string comparison |
| `randomBytes(n)` | cryptographically secure bytes |

## Tests

```
ez test.ez
```

65 assertions. Most run at a reduced cost so the suite finishes; the default is
exercised where the default itself is what is under test.

## License

MIT
