# ratelimit

Request throttling for EZ. Sliding window and token bucket, in-memory or
Redis-backed, with the conventional rate-limit headers.

`retry` protects you from a service that is failing. This is the other
direction: protecting your service from whoever is calling it.

```ez
use "ratelimit"

limiter = RateLimit(100, 60)                  # 100 requests per 60 seconds

result = limiter.check(req["ip"])
when result["allowed"] == false {
    give { "status": 429, "headers": limiter.headers(result) }
}
```

## Install

```
ez install ratelimit
```

No dependencies. Redis support works by duck-typing a client you pass in, so
installing this does not pull in `redis`.

## The result

Every `check` returns the same shape:

| field        | meaning                                                    |
|--------------|------------------------------------------------------------|
| `allowed`    | `true` if the request may proceed                           |
| `remaining`  | requests left in the current window                         |
| `limit`      | the configured limit, so you can build headers from this alone |
| `retryAfter` | seconds until the next request is possible; `0` when allowed |
| `resetAt`    | unix seconds at which capacity returns                      |

## Two algorithms

### Sliding window (default)

Counts what actually happened in the last N seconds. Strict, and the right
default for an API.

```ez
limiter = RateLimit(5, 60)                    # exactly 5 per rolling minute
```

Capacity returns gradually as old requests age out, rather than all at once.

A fixed window is deliberately not offered: it lets through twice the limit
across a boundary — the full quota at 0:59 and again at 1:00 — which is the
failure people discover in production.

### Token bucket

Refills continuously and allows a burst up to its capacity. Right where
occasional spikes are legitimate.

```ez
limiter = RateLimit(10, 10).bucket()          # 1/sec sustained, 10 at once
```

A client that has been idle may spend its whole bucket at once; one that is
steady is smoothed to the average rate. Refill is capped at capacity, so
idling does not bank unlimited credit.

## Costs

Not every endpoint is equally expensive.

```ez
limiter.consume(userId, 1)                    # a normal read
limiter.consume(userId, 10)                   # a report that hits the database hard
```

A cost larger than the whole limit could never succeed, so it throws rather
than quietly always returning `false`.

## Headers

```ez
limiter.headers(result)
```

```
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 0
X-RateLimit-Reset: 1786537627
Retry-After: 43
```

`Retry-After` appears only on a refusal, and is rounded up — telling a client
to retry in 0 seconds invites a hot loop.

## Inspecting without consuming

```ez
quota = limiter.peek(userId)                  # for a "requests left" display
limiter.reset(userId)                         # forgive one key
limiter.clear()                               # forget everything
```

## Guarding a call

```ez
limiter.guard(req["ip"],
    || handleRequest(req),
    |r| { "status": 429, "headers": limiter.headers(r) })
```

## Sharing state between workers

A limiter is only as good as the state behind it. Two web workers each holding
their own counters enforce twice the limit you asked for.

```ez
use "redis"

limiter = RateLimit(100, 60).useRedis(Redis("localhost", 6379), "api")
```

The client is duck-typed: anything with `get(key)`, `setEx(key, value, ttl)`
and `delete(key)` works, which also makes it easy to fake in tests. Records are
stored as JSON under a prefixed key with a TTL, so keys nobody touches again
expire on their own.

`clear()` throws on the Redis store — clearing every limiter key means scanning
the keyspace, which blocks the server. Delete the prefix yourself if you
really need to.

## Custom storage

```ez
limiter.useStore(myStore)
```

A store needs `read(key)`, `write(key, record)`, `forget(key)`, `clear()`,
`prune(now)` and `size()`.

## Memory

The in-memory store prunes expired records every 200 checks. Without that, a
limiter keyed on anything user-supplied — an IP, an API key — grows without
bound. Tune with `limiter.pruneEvery`, or set it to `0` to disable and prune
yourself.

## Shorthands

```ez
perSecond(10)
perMinute(100)
perHour(1000)
```

## API

**`RateLimit(limit, windowSeconds)`**

| method                     | does                                              |
|----------------------------|---------------------------------------------------|
| `check(key)`               | consume one unit, return the result               |
| `consume(key, cost)`       | consume `cost` units                              |
| `peek(key)`                | report without consuming                          |
| `reset(key)`               | forget one key                                    |
| `clear()`                  | forget every key                                  |
| `headers(result)`          | the conventional response headers                 |
| `guard(key, action, rejected)` | run one or the other                          |
| `sliding()` / `bucket()`   | choose the algorithm; both return `self`          |
| `useRedis(client, prefix)` | share state between processes                     |
| `useStore(store)`          | use your own storage                              |

## Tests

```
ez test.ez
```

66 assertions, including that the window really slides and that two limiters
sharing one client share the limit.

## License

MIT
