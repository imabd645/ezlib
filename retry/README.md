# retry — retries, backoff and circuit breaking for EZ

> **Import:** `use "retry"`
> **Install:** `ez install retry`

```ez
use "retry"

response = retry(|| http.get(url))
```

Three attempts, exponential backoff, jitter. For anything that fails
transiently — a dropped connection, a 503, a lock timeout.

---

## Retry the right failures

A call fails for two different reasons. Something **transient** — a dropped
connection, a rate limit, a deadlock — is worth repeating. Something
**permanent** — a 404, bad credentials, malformed input — is not: retrying
wastes time, and on a write it can make things worse.

```ez
# Retry timeouts, give up immediately on anything else
policy = Retry()
    .times(5)
    .onlyWhen(|e| contains(e.message, "timeout"))

policy.run(|| fetchOrder(id))
```

Without `onlyWhen`, every exception is retried.

## Jitter is on by default

Without it, every client that failed during an outage retries at the same
instant and knocks the service over again just as it recovers — the
thundering herd. Full jitter picks a random point in `[0, delay]`, which
spreads clients out far better than adding a small random fraction.

Turn it off with `.withoutJitter()` only when you want predictable timing,
such as in tests.

## API

### Building a policy

| Call | Effect |
| --- | --- |
| `Retry()` | 3 attempts, 0.5s base delay, doubling, 30s cap, jitter on |
| `.times(n)` | Total attempts including the first. `times(1)` never retries |
| `.waiting(seconds)` | Delay before the second attempt |
| `.backoff(factor)` | Growth between attempts. `1` gives a constant delay |
| `.capped(seconds)` | Ceiling on any single delay |
| `.withoutJitter()` | Use the exact computed delay |
| `.onlyWhen(predicate)` | Retry only when the predicate says so |
| `.notify(handler)` | Called with `(attempt, error, delay)` before each wait |

All chainable.

### Running

| Call | Result |
| --- | --- |
| `p.run(action)` | The value, or rethrows the **last** exception |
| `p.runOr(action, fallback)` | The value, or `fallback` |
| `p.attempt(action)` | `{ ok, value, error, attempts }` |
| `p.delayFor(n)` | The delay before attempt `n`, for inspection |

`run` rethrows the original exception rather than a wrapper, so the caller
still learns what actually went wrong.

### Shorthands

```ez
retry(|| work())                   # 3 attempts with backoff
retryTimes(5, || work())
retryOr(|| work(), fallback)
networkRetry()                     # 5 attempts, 0.25s base, 10s cap
```

### Logging retries

A retry that nobody sees is just unexplained latency:

```ez
Retry().notify(|attempt, error, delay| {
    Log.warn(`attempt {attempt} failed: {error.message}, retrying in {delay}s`)
    give nil
}).run(|| work())
```

## Circuit breaker

Retrying a service that is genuinely down turns one slow request into many. A
breaker notices repeated failure and fails fast for a while, which spares the
caller and gives the service room to recover.

```ez
breaker = CircuitBreaker(5, 30)     # open after 5 failures, retry after 30s

result = breaker.runOr(|| callService(), cachedValue)
```

| State | Meaning |
| --- | --- |
| `closed` | Passing calls through normally |
| `open` | Failing immediately without calling the action |
| `half-open` | Cooldown elapsed; the next call is a trial |

A success in half-open closes it; a failure re-opens it. A success at any
point clears the failure count, so intermittent errors never trip it.

| Call | Effect |
| --- | --- |
| `b.run(action)` | Run unless open; throws immediately when open |
| `b.runOr(action, fallback)` | Same, with a fallback |
| `b.currentState()` / `b.isOpen()` | Inspect |
| `b.reset()` | Force closed |

Combine them: a breaker around a retry policy stops the retries themselves
from becoming the load.

## Notes

- **Delays are a busy-wait.** EZ has no sleep that is safe from every
  context, and retry delays are short by design. Do not set a delay of
  minutes — that belongs in a scheduler, not here.
- **Retries are synchronous.** The calling thread blocks.
- **Only exceptions trigger a retry.** A function that returns an error value
  instead of throwing will not be retried; throw, or check the value yourself.
- **Not safe for non-idempotent writes** unless you know the operation can be
  repeated. A retried POST may create two records.

## Testing

```
ez test.ez
```

46 tests covering attempt counts, the original error surviving, conditional
retrying, notification, backoff shape and capping, jitter bounds, and every
circuit-breaker transition including cooldown to half-open.

## License

MIT
