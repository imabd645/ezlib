# thread

Concurrency helpers for EZ: parallel iteration, bounded worker pools, timeouts,
retries and rate limiting.

```ez
use "thread"

results = parallelMap([1, 2, 3], | n | { give n * 2 })

pool = WorkerPool(4)
pool.mapAll(urls, fetchOne)        # never more than 4 in flight
```

## Use the interpreter directly for the basics

These are **builtins**. This package does not wrap them:

| Builtin | Purpose |
| --- | --- |
| `spawn(fn, args…)` | Run on another thread → future |
| `await(f)` / `sync(f)` | Wait for one future |
| `awaitAll(futures)` | Wait for all, results in order |
| `awaitAny(futures)` | First to **settle** |
| `isDone(f)` | Has it finished? Non-blocking |
| `cancel(f)` | Cancel a future |
| `wait(ms)` | Sleep |
| `mutex()` / `lock(m, fn)` | Mutual exclusion, RAII release |
| `Channel()` | Blocking queue |
| `Atomic(v)` | Atomic counter |

### What was removed, and why

| Removed | Use instead |
| --- | --- |
| `all(futures)` | `awaitAll(futures)` |
| `race(futures)` | `awaitAny(futures)` |
| `sleep(ms)`, `delay(ms)`, `delayTask(ms)` | `wait(ms)` |
| `Mutex` model | `mutex()` / `lock(m, fn)` |
| `Channel` model | native `Channel()` |

The sync ones were not merely redundant — they were **unsafe**. The old `Mutex`
and `Semaphore` did:

```ez
while self.state["locked"] { wait(1) }
self.state["locked"] = true          # separate step
```

The check and the set are not atomic, so two threads could both see the lock
free and both take it. That is no mutual exclusion at all, and `WorkerPool`
built on it did not actually bound anything. The old `Channel` was an
unsynchronised array polled on a timer, shadowing the native class. The
replacements are built on the interpreter's real mutexes and condition
variables.

## Parallel iteration

```ez
parallelMap(items, mapper)                    # results in input order
parallelFilter(items, predicate)
parallelForEach(items, fn)
parallelMapBatched(items, mapper, batchSize=8)
times(n, fn)                                  # fn(0) … fn(n-1)
sequential(fns)                               # one after another, this thread
```

`parallelMap` is **unbounded** — 10,000 items starts 10,000 threads. Use
`WorkerPool` or `parallelMapBatched` for large inputs.

## TaskGroup

```ez
g = createGroup()
g.add(fetchOne, url)
g.addTask(| | { give compute() })
g.waitAll()        # results in order, throws on first failure
g.waitSettled()    # every outcome, never throws
g.cancelAll()
```

## Futures

| Function | Behaviour |
| --- | --- |
| `allSettled(futures)` | `[{ok, value, error}, …]` — never throws, keeps successes |
| `any(futures)` | First **success**; throws `AllFailedError` only if all fail |
| `withTimeout(future, ms)` | Value, or `TimeoutError` |
| `timeoutFn(fn, ms)` | Run `fn()` with a deadline |
| `retry(fn, maxAttempts=3, delayMs=0)` | Retry until it works |
| `retryBackoff(fn, maxAttempts=5, baseDelayMs=50, maxDelayMs=5000)` | Doubling pause |
| `deferred()` | A future you complete by hand |
| `pending(futures)` / `settledSoFar(futures)` | Progress, non-blocking |

`any()` differs from native `awaitAny`: `awaitAny` returns the first future to
**settle** — if that one failed, it fails. `any()` skips failures and returns
the first that **succeeded**.

`withTimeout` ends the *wait*, not the work. The underlying task keeps running.

```ez
d = deferred()
spawn(| | { wait(50) d.resolve(9) })
await(d.future)      # 9
```

## Synchronisation

```ez
sem = Semaphore(2)
sem.acquire()                 # blocks until a permit is free
sem.acquireTimeout(500)       # or throws TimeoutError
sem.tryAcquire()              # true/false, never blocks
sem.release()
sem.withPermit(| | { … })     # releases even if the body throws
sem.available()               # permits free
```

Permits are tokens in a native `Channel`, so taking one is a single atomic
operation and waiting blocks in the OS rather than polling.

```ez
pool = WorkerPool(4)
pool.mapAll(items, mapper)    # results in input order
pool.forEach(items, fn)
```

```ez
latch = Latch()               # one-shot gate
spawn(| | { latch.await() ; go() })
latch.open()                  # every waiter proceeds
```

## Rate limiting

```ez
th = throttle(fn, 1000)       # at most one call per second; extras return nil
db = debounce(fn, 300)        # fires 300ms after the last call
m  = memoize(fn)              # caches by str(arg)

rl = RateLimiter(100, 60000)  # 100 calls per minute, blocks rather than drops
rl.run(| | { callApi() })
```

`memoize` has no expiry and no size bound — do not point it at unbounded input.

## Errors

`ThreadError`, `TimeoutError`, `AllFailedError` (carries `.errors`, the
individual failures).

## Layout

| File | Contents |
| --- | --- |
| `main.ez` | Entry, progress helpers |
| `futures.ez` | `allSettled`, `any`, `withTimeout`, `retry`, `Deferred` |
| `parallel.ez` | `parallelMap` and friends, `TaskGroup` |
| `sync.ez` | `Semaphore`, `WorkerPool`, `Latch` |
| `rate.ez` | `throttle`, `debounce`, `memoize`, `RateLimiter` |
| `errors.ez` | Error models |

## Tests

```
ez thread/test_thread.ez
```
