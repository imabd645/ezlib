# ezdatetime — Date & Time Library for EZ

> **Version:** 2.0.0  
> **Import:** `use "ezdatetime"`  
> **File:** `E:\ezlib\ezdatetime\main.ez`

---

## Overview

`ezdatetime` provides a complete, pure-EZ date and time library. It works with **Unix timestamps in milliseconds** (ms since `1970-01-01T00:00:00 UTC`) and provides decomposition, formatting, arithmetic, and comparison utilities.

All timestamps are UTC-based. To display local time, add your timezone offset in hours using `addTime()` or the `dateTime()` / `timeNow()` helpers.

---

## Quick Start

```ez
use "ezdatetime"

ts = now()
out format(ts)    # → "2025-06-11 08:30:00"
out dateNow()     # → "2025-06-11"
out timeNow(5)    # → "13:30:00" (UTC+5)
```

---

## Constants

| Constant | Value | Description |
|---|---|---|
| `MS_PER_SEC` | `1000` | Milliseconds in 1 second |
| `MS_PER_MIN` | `60000` | Milliseconds in 1 minute |
| `MS_PER_HOUR` | `3600000` | Milliseconds in 1 hour |
| `MS_PER_DAY` | `86400000` | Milliseconds in 1 day |

---

## Core Functions

### `now()` → `number`
Returns the current Unix timestamp in milliseconds.

```ez
use "ezdatetime"

ts = now()
out ts   # → e.g. 1749556800000
```

Internally wraps the EZ built-in `clock()`.

---

### `format(ts)` → `string`
Formats a timestamp as `"YYYY-MM-DD HH:MM:SS"`.

```ez
use "ezdatetime"

ts = makeTimestamp(2025, 6, 11, 14, 30, 0)
out format(ts)   # → "2025-06-11 14:30:00"
```

Hours, minutes, and seconds are zero-padded to 2 digits.

---

### `dateStr(ts)` → `string`
Returns only the date portion as `"YYYY-MM-DD"`.

```ez
use "ezdatetime"

ts = now()
out dateStr(ts)  # → "2025-06-11"
```

---

### `timeStr(ts)` → `string`
Returns only the time portion as `"HH:MM:SS"`.

```ez
use "ezdatetime"

ts = makeTimestamp(2025, 1, 1, 9, 5, 3)
out timeStr(ts)  # → "09:05:03"
```

---

### `dateNow()` → `string`
Shorthand: returns today's UTC date as `"YYYY-MM-DD"`.

```ez
use "ezdatetime"

out dateNow()  # → "2025-06-11"
```

---

### `timeNow(offsetHours)` → `string`
Returns the current time with an optional UTC offset in hours as `"HH:MM:SS"`.

**Parameters:**
- `offsetHours` — Integer hours to add (positive = ahead of UTC, negative = behind).

```ez
use "ezdatetime"

out timeNow(0)   # → "08:30:00" UTC
out timeNow(5)   # → "13:30:00" UTC+5 (Pakistan Standard Time)
out timeNow(-5)  # → "03:30:00" UTC-5 (Eastern Standard Time)
```

---

### `dateTime(offsetHours)` → `string`
Returns the full formatted date-time string `"YYYY-MM-DD HH:MM:SS"` with a UTC hour offset.

```ez
use "ezdatetime"

out dateTime(5)   # → "2025-06-11 13:30:00" (UTC+5)
```

---

## Component Decomposition

### `getComponents(ts)` → `dictionary`
Decomposes a Unix millisecond timestamp into its date and time components.

**Returns:** Dictionary with fields:
- `year` — Full year (e.g. `2025`)
- `month` — Month (1–12)
- `day` — Day of month (1–31)
- `hour` — Hour (0–23)
- `minute` — Minute (0–59)
- `second` — Second (0–59)
- `millisecond` — Milliseconds (0–999)
- `weekday` — Day of week: `0=Sunday, 1=Monday, ..., 6=Saturday`

**Examples:**
```ez
use "ezdatetime"

ts = makeTimestamp(2025, 3, 15, 10, 45, 30)
c = getComponents(ts)

out c.year       # → 2025
out c.month      # → 3
out c.day        # → 15
out c.hour       # → 10
out c.minute     # → 45
out c.second     # → 30
out c.weekday    # → 6 (Saturday)
```

---

## Timestamp Construction

### `makeTimestamp(y, m, d, h, mn, s)` → `number`
Creates a Unix millisecond timestamp from date/time components.

**Parameters:**
- `y` — Year (e.g. `2025`)
- `m` — Month (1–12)
- `d` — Day (1–31)
- `h` — Hour (0–23)
- `mn` — Minute (0–59)
- `s` — Second (0–59)

**Returns:** Unix timestamp in milliseconds (UTC).

**Examples:**
```ez
use "ezdatetime"

ts = makeTimestamp(2025, 12, 25, 0, 0, 0)
out format(ts)   # → "2025-12-25 00:00:00"

birthday = makeTimestamp(1990, 7, 4, 12, 0, 0)
out dateStr(birthday)  # → "1990-07-04"

# Epoch itself
epoch = makeTimestamp(1970, 1, 1, 0, 0, 0)
out epoch  # → 0
```

---

## Arithmetic Functions

### `addTime(ts, days, hours, mins, secs)` → `number`
Adds an arbitrary duration to a timestamp.

**Parameters:**
- `ts` — Base timestamp in milliseconds.
- `days` — Days to add.
- `hours` — Hours to add.
- `mins` — Minutes to add.
- `secs` — Seconds to add.

Use negative values to subtract time.

**Examples:**
```ez
use "ezdatetime"

now = makeTimestamp(2025, 6, 11, 12, 0, 0)

# Add 1 day
tomorrow = addTime(now, 1, 0, 0, 0)
out dateStr(tomorrow)  # → "2025-06-12"

# Add 2 hours 30 minutes
later = addTime(now, 0, 2, 30, 0)
out timeStr(later)   # → "14:30:00"

# Subtract 7 days (go back a week)
lastWeek = addTime(now, -7, 0, 0, 0)
out dateStr(lastWeek)  # → "2025-06-04"
```

---

### `diffMs(ts1, ts2)` → `number`
Returns the difference `ts1 - ts2` in milliseconds.

```ez
use "ezdatetime"

a = makeTimestamp(2025, 6, 11, 10, 0, 0)
b = makeTimestamp(2025, 6, 11, 8, 0, 0)

diff = diffMs(a, b)
out diff          # → 7200000 (2 hours in ms)
out diff / 3600000 # → 2.0
```

---

### `diffDays(ts1, ts2)` → `number`
Returns the number of full days between two timestamps (`ts1 - ts2` in days).

```ez
use "ezdatetime"

start = makeTimestamp(2025, 1, 1, 0, 0, 0)
end = makeTimestamp(2025, 12, 31, 0, 0, 0)

out diffDays(end, start)  # → 364
```

---

## Helper Functions

### `isLeap(year)` → `boolean`
Returns `true` if the given year is a leap year (divisible by 4, except centuries unless divisible by 400).

```ez
use "ezdatetime"

out isLeap(2024)  # → true
out isLeap(2025)  # → false
out isLeap(1900)  # → false (century, not 400-divisible)
out isLeap(2000)  # → true  (400-divisible)
```

---

### `daysInMonth(year, month)` → `number`
Returns the number of days in the given month, accounting for leap years.

```ez
use "ezdatetime"

out daysInMonth(2024, 2)  # → 29 (leap year)
out daysInMonth(2025, 2)  # → 28
out daysInMonth(2025, 4)  # → 30 (April)
out daysInMonth(2025, 1)  # → 31 (January)
```

---

### `padZero(num)` → `string`
Zero-pads a number to at least 2 digits.

```ez
use "ezdatetime"

out padZero(5)   # → "05"
out padZero(12)  # → "12"
out padZero(0)   # → "00"
```

---

### `floorDiv(a, b)` → `number`
Integer floor division.

```ez
use "ezdatetime"

out floorDiv(7, 3)    # → 2
out floorDiv(-7, 3)   # → -3 (floor, not truncate)
```

---

## Edge Cases & Important Notes

### UTC Only
All timestamps are UTC. There is no built-in timezone database. To convert to local time, manually add your UTC offset in hours:
```ez
use "ezdatetime"

# Pakistan Standard Time (UTC+5)
localTs = addTime(now(), 0, 5, 0, 0)
out format(localTs)
```

### Negative Timestamps (Before 1970)
Dates before Jan 1, 1970 would require negative timestamps. `getComponents()` is not designed for negative input — avoid calling it with `ts < 0`.

### Month/Day Boundaries
`addTime()` does not validate calendar boundaries. Adding 31 days to January 31 gives March 3 (or 4 in a leap year), not a date error. This is correct behavior (raw millisecond addition).

### Weekday Values
`weekday` in `getComponents()` is `0=Sunday` through `6=Saturday`, matching the JavaScript convention. `1970-01-01` was a Thursday (4).

### Large Year Handling
`getComponents()` iterates year-by-year through a `while` loop. Timestamps very far in the future (e.g., year 3000+) will be slow to decompose due to the linear iteration.

### Millisecond Precision
All operations are in milliseconds. The `millisecond` field in `getComponents()` gives the sub-second remainder.

---

## Full Example: Age Calculator

```ez
use "ezdatetime"

task calculateAge(birthYear, birthMonth, birthDay) {
    birthTs = makeTimestamp(birthYear, birthMonth, birthDay, 0, 0, 0)
    currentTs = now()
    
    days = diffDays(currentTs, birthTs)
    years = floor(days / 365.25)
    months = floor((days % 365.25) / 30.44)
    
    out "Age: " + str(years) + " years, " + str(months) + " months"
    out "That is " + str(days) + " days old!"
}

calculateAge(1995, 3, 20)
```

---

## Full Example: Countdown Timer Display

```ez
use "ezdatetime"

task countdown(targetYear, targetMonth, targetDay) {
    targetTs = makeTimestamp(targetYear, targetMonth, targetDay, 0, 0, 0)
    currentTs = now()
    
    diff = diffMs(targetTs, currentTs)
    
    when diff <= 0 {
        out "Event has already passed!"
        give
    }
    
    days = floor(diff / MS_PER_DAY)
    remaining = diff % MS_PER_DAY
    hours = floor(remaining / MS_PER_HOUR)
    remaining = remaining % MS_PER_HOUR
    minutes = floor(remaining / MS_PER_MIN)
    
    out "Time until event:"
    out "  " + str(days) + " days"
    out "  " + str(hours) + " hours"
    out "  " + str(minutes) + " minutes"
}

countdown(2025, 12, 25)  # Days until Christmas
```

---

## Full Example: Event Log Timestamping

```ez
use "ezdatetime"

events = []

task logEvent(name) {
    push(events, {
        "name": name,
        "time": now(),
        "formatted": dateTime(5)  # UTC+5
    })
}

logEvent("User login")
wait(100)
logEvent("Page load")
wait(200)
logEvent("User logout")

get e in events {
    out "[" + e["formatted"] + "] " + e["name"]
}
```

---

## Full Example: Date Range Iterator

```ez
use "ezdatetime"

task eachDay(startYear, startMonth, startDay, endYear, endMonth, endDay) {
    current = makeTimestamp(startYear, startMonth, startDay, 0, 0, 0)
    end = makeTimestamp(endYear, endMonth, endDay, 0, 0, 0)
    
    days = []
    while current <= end {
        push(days, dateStr(current))
        current = addTime(current, 1, 0, 0, 0)
    }
    give days
}

range = eachDay(2025, 6, 1, 2025, 6, 7)
get d in range {
    out d
}
# → 2025-06-01
# → 2025-06-02
# → ...
# → 2025-06-07
```

---

*Documentation generated from `E:\ezlib\ezdatetime\main.ez` — EZ DateTime Library v2.0.0*
