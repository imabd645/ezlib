# cron

Recurring schedules for EZ. Cron expression parsing, next-occurrence
calculation, and an in-process scheduler with failure handling.

```ez
use "cron"

jobs = Scheduler()
jobs.add("nightly-report", "0 3 * * *", || sendReport())
jobs.add("heartbeat", "@every 30s", || ping())
jobs.run()
```

Or just the arithmetic:

```ez
schedule = Cron("0 9 * * MON-FRI")
schedule.next()                                # the next unix timestamp
schedule.describe()                            # "at 09:00 on Monday, …"
```

## Install

```
ez install cron
```

No dependencies.

## Where this sits

`queue` runs a job once, possibly after a delay. This is the other thing every
service needs: something that happens *again*, on a calendar.

`taskschd` drives the Windows Task Scheduler — a different tool for a different
job. It schedules processes, from outside, and needs the machine configured.
This runs inside your program and needs nothing.

## Expressions

Five fields, as in any crontab:

```
 ┌───── minute        0-59
 │ ┌───── hour        0-23
 │ │ ┌───── day       1-31
 │ │ │ ┌───── month   1-12 or JAN-DEC
 │ │ │ │ ┌───── weekday 0-6 or SUN-SAT (7 is also Sunday)
 │ │ │ │ │
 * * * * *
```

Six fields puts seconds on the front: `*/30 * * * * *` is every thirty seconds.

| syntax | means |
|---|---|
| `*` | every value |
| `5` | exactly 5 |
| `1,3,5` | any of those |
| `2-6` | that range |
| `*/15` | every 15th, from the start |
| `10-20/5` | every 5th within the range |
| `55/2` | every 2nd from 55 to the end |
| `MON-FRI` | by name |
| `FRI-MON` | wrapping round the end |
| `?` | same as `*` |

Tabs and doubled spaces are fine — a copy-pasted crontab line should just work.

### Macros

`@yearly` `@annually` `@monthly` `@weekly` `@daily` `@midnight` `@hourly`
`@minutely` `@secondly`

And a fixed interval, which is not a calendar position at all:

```
@every 30s    @every 5m    @every 2h    @every 1d
```

## The day-of-month / day-of-week rule

The one thing about cron that surprises everybody: when **both** day fields are
restricted, the day matches if **either** does — not both.

```ez
Cron("0 0 1 * MON")     # the 1st of the month AND every Monday
```

That is POSIX, and every crontab in existence behaves this way. Changing it
here would silently reschedule anyone's existing expression, so it is
implemented as specified and documented rather than fixed.

With only one of them restricted, the other is not consulted at all.

## Time zones

Everything is UTC.

A fixed offset is available:

```ez
Cron("0 3 * * *").withOffset(330)              # 03:00 IST
```

Minutes, so India (+330) and Nepal (+345) work.

DST-aware local scheduling is deliberately not attempted. "03:00 every day" is
genuinely ambiguous across a transition — it happens twice on one day of the
year and not at all on another — and a library that silently picks one is worse
than one that tells you it does not. Schedule in UTC, or apply the offset you
mean.

## Next occurrences

```ez
schedule.next()                                # after now
schedule.nextAfter(timestamp)                  # after a given moment
schedule.nextRuns(5)                           # the next five
schedule.matches(timestamp)                    # does this exact second match?
schedule.secondsUntilNext()
```

`next` is strictly after the moment given, so a schedule that has just fired
does not fire again.

The search is bounded at ten years. The longest real gap between occurrences is
February 29th across a skipped century — 2096 to 2104 is eight years, because
2100 is not a leap year. An expression that can never match, like
`0 0 30 2 *`, says so instead of hanging.

## Describing a schedule

```ez
describe("0 9 * * MON-FRI")
# "at 09:00 on Monday, Tuesday, Wednesday, Thursday, Friday"
```

A schedule you cannot read is a schedule you cannot review. Show it back to
whoever configured it.

## The scheduler

```ez
jobs = Scheduler()
jobs.add("cleanup", "0 4 * * *", || purgeOldSessions())
jobs.onFailure(|name, err| logError(name, err))
jobs.run()
```

| method | does |
|---|---|
| `add(name, expression, action)` | register a job; names must be unique |
| `remove(name)` | `true` if it was there |
| `enable(name)` / `disable(name)` | without losing the job |
| `job(name)` | the job's record |
| `list()` | every job with its next run, counts and failures |
| `tick(now)` | run what is due; returns how many fired |
| `run()` / `runFor(seconds)` | block and keep ticking |
| `stop()` | end a running loop |
| `secondsUntilNext()` | for your own sleep |
| `onFailure(handler)` | called with `(name, error)` |

### What it does when things go wrong

**A job that throws does not stop the scheduler.** The failure is counted, kept
on the job, passed to `onFailure`, and the job is rescheduled normally. It is
not retried immediately — if you want retries, wrap the action in `retry`.

**A job that overruns does not then storm.** The next run is computed from the
moment it was *due*, so a slow job does not drift its own schedule later and
later. But if it ran so long that it missed occurrences entirely, it does not
fire once per interval catching up — it resumes from now.

**Re-enabling a paused job does not fire for everything it slept through.**

If you have your own event loop, skip `run()` and call `tick()` from it.

## API

Also exported: `parse(expression)`, `nextRun(expression)`,
`describe(expression)`, `isValid(expression)`, and from `cron/time.ez` the
calendar helpers `buildUp`, `breakDown`, `formatTime`, `isLeapYear`,
`daysInMonth`, `weekdayFromDays`, `nowSeconds`.

## Tests

```
ez test.ez
```

155 assertions. The calendar is pinned first against timestamps that can be
checked by hand — the epoch, Y2K, the 2038 overflow, a leap day — because every
schedule assertion depends on it.

## License

MIT
