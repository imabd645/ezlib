# queue — background jobs for EZ

> **Import:** `use "queue"`
> **Install:** `ez install queue`
> **Depends on:** `redis`

Work that does not have to finish inside a request — sending mail, resizing an
image, calling a slow API — belongs on a queue.

```ez
use "queue"

q = Queue()
q.handle("email", |payload| sendEmail(payload))

q.push("email", { "to": "a@b.co" })
q.work()
```

---

## Two drivers

**Memory** (the default) keeps jobs in this process. Right for tests,
single-process tools and development; everything is lost on restart.

**Redis** persists jobs and lets several workers share one queue — what you
want in production.

```ez
use "redis"

client = Redis("127.0.0.1", 6379)
q = redisQueue(client, "myapp:jobs")
```

The API is identical either way, so you can develop against memory and deploy
against Redis.

## Producing and consuming

Typically two programs: the app queues jobs, a worker runs them.

```ez
# In the request handler
q.push("resize", { "path": upload })

# In a separate worker process
q.handle("resize", |p| resizeImage(p["path"]))
q.workFor(60)          # process for a minute, then exit
```

| Call | Effect |
| --- | --- |
| `q.push(name, payload)` | Queue a job. Returns its id |
| `q.later(name, payload, seconds)` | Queue it to become available later |
| `q.work()` | Run until the queue is empty. Returns the count |
| `q.workLimit(n)` | Run at most `n` jobs |
| `q.workFor(seconds)` | Keep processing, waiting when idle |
| `q.runNext()` | Run one job if ready; nil when idle |
| `q.size()` / `q.isEmpty()` / `q.clear()` | Inspect and empty |
| `q.failed()` | Jobs that exhausted their attempts |

Payloads are dictionaries and survive nesting and lists intact. With the Redis
driver they are serialised as JSON, so keep them to data — not model
instances or functions.

## Retries

A failing job is retried up to `attempts` times, then moved to the failed
list with the exception message attached.

```ez
q = Queue().attempts(5).backoff(2)      # 5 tries, 2s between
```

`attempts(1)` disables retrying. A job with **no registered handler** fails
immediately rather than burning its attempts — retrying will not conjure a
handler, and consuming the budget would hide the real problem.

```ez
q.whenFailed(|job, reason| Log.error(`job {job["name"]} failed: {reason}`))
q.whenCompleted(|job| Log.info(`job {job["id"]} done`))
```

Inspect what failed:

```ez
get job in q.failed() {
    out job["name"] + ": " + job["error"]
}
```

## Named queues

Different work deserves different rates — a slow report should not hold up
password-reset emails.

```ez
emails = redisQueue(client, "app").on("emails")
reports = redisQueue(client, "app").on("reports")
```

Each queue is processed independently even when they share a driver.

## Recovering after a crash

When the Redis driver reserves a job it also records it in a *processing*
list, so a worker that dies mid-job leaves a trace rather than losing the work
silently.

```ez
recovered = q.recoverStalled()      # put them back
```

This is **deliberately manual** — call it on startup, not on a timer.
Automatically re-running a job that may have half-completed is not always
safe, and a job that is merely slow would be run twice.

For the memory driver it returns 0: a dead process loses everything anyway.

## Limits

- **No parallelism.** Jobs run one at a time on the calling thread. For
  concurrency, run several worker processes against the same Redis queue.
- **No priorities within a queue.** Use separate named queues instead.
- **At-least-once delivery.** A worker that dies after finishing a job but
  before recording it will re-run that job when recovered. Make handlers
  idempotent where it matters.
- **`workFor` busy-waits when idle**, at a low duty cycle. Fine for a
  dedicated worker, wasteful if you share the core.
- Delayed jobs are promoted when a worker looks for work, so nothing runs if
  no worker is running.

## Testing

```
ez test.ez
```

38 tests against the memory driver: ordering, payload fidelity, retry
exhaustion, recovery on a later attempt, unhandled jobs, callbacks, delayed
jobs, named queue isolation and clearing. A further 8 run against Redis when
one is reachable on `127.0.0.1:6379`, and report as skipped otherwise.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Queue` model: handlers, processing loops, retry policy |
| `memory.ez` | In-process driver |
| `redisdriver.ez` | Redis driver: lists, delayed jobs, stalled-job recovery |

## License

MIT
