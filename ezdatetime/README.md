# DateTime Library for EZ Language

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/yourusername/ez-datetime)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A comprehensive date and time manipulation library for the EZ programming language. This library provides utilities for working with timestamps, formatting dates, performing date arithmetic, and extracting date components.

## Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [API Reference](#api-reference)
  - [Constants](#constants)
  - [Current Time Functions](#current-time-functions)
  - [Date Component Functions](#date-component-functions)
  - [Timestamp Creation](#timestamp-creation)
  - [Formatting Functions](#formatting-functions)
  - [Date Arithmetic](#date-arithmetic)
  - [Helper Functions](#helper-functions)
- [Usage Examples](#usage-examples)
- [Contributing](#contributing)
- [License](#license)

## Installation

Copy the `datetime.ez` file into your project directory and import it into your EZ programs.

```ez
// Import the datetime library
load "datetime.ez"
```

## Quick Start

```ez
load "datetime.ez"

// Get current timestamp
current = now()
say "Current timestamp: " + str(current)

// Format current date and time
say "Now: " + format(current)

// Get just the date
say "Today: " + dateNow()

// Create a specific timestamp
birthday = makeTimestamp(1990, 5, 15, 14, 30, 0)
say "Birthday: " + format(birthday)

// Add time to a timestamp
future = addTime(current, 7, 0, 0, 0)  // Add 7 days
say "Next week: " + format(future)

// Calculate difference between dates
days = diffDays(future, current)
say "Days difference: " + str(days)
```

## Core Concepts

### Unix Timestamp

This library uses **Unix timestamps** measured in **milliseconds** since January 1, 1970 00:00:00 UTC (the Unix epoch). This is different from the standard Unix timestamp which uses seconds.

**Example:**
- `0` = January 1, 1970 00:00:00 UTC
- `1000` = January 1, 1970 00:00:01 UTC
- `86400000` = January 2, 1970 00:00:00 UTC

### Weekday Numbering

Weekdays are numbered 0-6:
- `0` = Sunday
- `1` = Monday
- `2` = Tuesday
- `3` = Wednesday
- `4` = Thursday
- `5` = Friday
- `6` = Saturday

## API Reference

### Constants

The library provides several time conversion constants:

| Constant | Value | Description |
|----------|-------|-------------|
| `MS_PER_SEC` | 1000 | Milliseconds in one second |
| `MS_PER_MIN` | 60000 | Milliseconds in one minute |
| `MS_PER_HOUR` | 3600000 | Milliseconds in one hour |
| `MS_PER_DAY` | 86400000 | Milliseconds in one day |

**Example:**
```ez
three_hours = 3 * MS_PER_HOUR  // 10800000 milliseconds
```

---

### Current Time Functions

#### `now()`

Returns the current timestamp in milliseconds since Unix epoch.

**Returns:** `Number` - Current timestamp

**Example:**
```ez
current_time = now()
say "Current timestamp: " + str(current_time)
```

---

#### `dateNow()`

Returns the current date as a formatted string (YYYY-MM-DD).

**Returns:** `String` - Current date in ISO format

**Example:**
```ez
today = dateNow()
say "Today's date: " + today  // "2024-02-03"
```

---

#### `timeNow(x)`

Returns the current time plus `x` hours, formatted as HH:MM:SS.

**Parameters:**
- `x` (Number) - Hours to add to current time

**Returns:** `String` - Time in HH:MM:SS format

**Example:**
```ez
current = timeNow(0)      // Current time: "14:30:45"
in_3_hours = timeNow(3)   // Time in 3 hours: "17:30:45"
```

---

### Date Component Functions

#### `getComponents(ts)`

Extracts all date and time components from a timestamp.

**Parameters:**
- `ts` (Number) - Unix timestamp in milliseconds

**Returns:** `Object` with the following properties:
- `year` (Number) - Four-digit year (e.g., 2024)
- `month` (Number) - Month (1-12)
- `day` (Number) - Day of month (1-31)
- `hour` (Number) - Hour (0-23)
- `minute` (Number) - Minute (0-59)
- `second` (Number) - Second (0-59)
- `millisecond` (Number) - Millisecond (0-999)
- `weekday` (Number) - Day of week (0-6, Sunday=0)

**Example:**
```ez
ts = now()
components = getComponents(ts)

say "Year: " + str(components.year)
say "Month: " + str(components.month)
say "Day: " + str(components.day)
say "Hour: " + str(components.hour)
say "Weekday: " + str(components.weekday)
```

---

### Timestamp Creation

#### `makeTimestamp(y, m, d, h, mn, s)`

Creates a Unix timestamp from individual date and time components.

**Parameters:**
- `y` (Number) - Year (e.g., 2024)
- `m` (Number) - Month (1-12)
- `d` (Number) - Day of month (1-31)
- `h` (Number) - Hour (0-23)
- `mn` (Number) - Minute (0-59)
- `s` (Number) - Second (0-59)

**Returns:** `Number` - Unix timestamp in milliseconds

**Example:**
```ez
// January 1, 2024 at 12:00:00
new_year = makeTimestamp(2024, 1, 1, 12, 0, 0)

// December 25, 2024 at 18:30:45
christmas = makeTimestamp(2024, 12, 25, 18, 30, 45)
```

---

### Formatting Functions

#### `format(ts)`

Formats a timestamp as a complete date-time string.

**Parameters:**
- `ts` (Number) - Unix timestamp in milliseconds

**Returns:** `String` - Formatted as "YYYY-MM-DD HH:MM:SS"

**Example:**
```ez
ts = makeTimestamp(2024, 7, 4, 16, 30, 0)
formatted = format(ts)
say formatted  // "2024-07-04 16:30:00"
```

---

#### `dateStr(ts)`

Formats a timestamp as a date-only string.

**Parameters:**
- `ts` (Number) - Unix timestamp in milliseconds

**Returns:** `String` - Formatted as "YYYY-MM-DD"

**Example:**
```ez
ts = now()
date = dateStr(ts)
say date  // "2024-02-03"
```

---

#### `timeStr(ts)`

Formats a timestamp as a time-only string.

**Parameters:**
- `ts` (Number) - Unix timestamp in milliseconds

**Returns:** `String` - Formatted as "HH:MM:SS"

**Example:**
```ez
ts = now()
time = timeStr(ts)
say time  // "14:30:45"
```

---

#### `dateTime(x)`

Returns the current date and time plus `x` hours, fully formatted.

**Parameters:**
- `x` (Number) - Hours to add to current time

**Returns:** `String` - Formatted as "YYYY-MM-DD HH:MM:SS"

**Example:**
```ez
now_str = dateTime(0)      // Current: "2024-02-03 14:30:00"
later = dateTime(5)        // In 5 hours: "2024-02-03 19:30:00"
```

---

### Date Arithmetic

#### `addTime(ts, days, hours, mins, secs)`

Adds or subtracts time from a timestamp.

**Parameters:**
- `ts` (Number) - Original timestamp in milliseconds
- `days` (Number) - Days to add (use negative to subtract)
- `hours` (Number) - Hours to add (use negative to subtract)
- `mins` (Number) - Minutes to add (use negative to subtract)
- `secs` (Number) - Seconds to add (use negative to subtract)

**Returns:** `Number` - New timestamp in milliseconds

**Example:**
```ez
ts = now()

// Add 7 days
next_week = addTime(ts, 7, 0, 0, 0)

// Subtract 2 hours and 30 minutes
earlier = addTime(ts, 0, -2, -30, 0)

// Add 1 day, 3 hours, 15 minutes, 30 seconds
future = addTime(ts, 1, 3, 15, 30)
```

---

#### `diffMs(ts1, ts2)`

Calculates the difference between two timestamps in milliseconds.

**Parameters:**
- `ts1` (Number) - First timestamp
- `ts2` (Number) - Second timestamp

**Returns:** `Number` - Difference in milliseconds (ts1 - ts2)

**Example:**
```ez
start = makeTimestamp(2024, 1, 1, 0, 0, 0)
end = makeTimestamp(2024, 1, 2, 0, 0, 0)

diff = diffMs(end, start)
say "Difference: " + str(diff) + " ms"  // 86400000 ms
```

---

#### `diffDays(ts1, ts2)`

Calculates the difference between two timestamps in whole days.

**Parameters:**
- `ts1` (Number) - First timestamp
- `ts2` (Number) - Second timestamp

**Returns:** `Number` - Difference in days (rounded down)

**Example:**
```ez
start = makeTimestamp(2024, 1, 1, 0, 0, 0)
end = makeTimestamp(2024, 1, 15, 0, 0, 0)

days = diffDays(end, start)
say "Days between: " + str(days)  // 14
```

---

### Helper Functions

#### `isLeap(year)`

Determines if a year is a leap year.

**Parameters:**
- `year` (Number) - Four-digit year

**Returns:** `Boolean` - `true` if leap year, `false` otherwise

**Example:**
```ez
when isLeap(2024) {
    say "2024 is a leap year"  // This will print
}

when isLeap(2023) {
    say "2023 is a leap year"  // This won't print
}
```

---

#### `daysInMonth(year, month)`

Returns the number of days in a given month, accounting for leap years.

**Parameters:**
- `year` (Number) - Four-digit year
- `month` (Number) - Month (1-12)

**Returns:** `Number` - Days in the month (28-31)

**Example:**
```ez
days_feb_2024 = daysInMonth(2024, 2)  // 29 (leap year)
days_feb_2023 = daysInMonth(2023, 2)  // 28
days_april = daysInMonth(2024, 4)     // 30
```

---

#### `padZero(num)`

Pads a single-digit number with a leading zero.

**Parameters:**
- `num` (Number) - Number to pad

**Returns:** `String` - Padded string

**Example:**
```ez
padded = padZero(5)   // "05"
normal = padZero(15)  // "15"
```

---

#### `floorDiv(a, b)`

Performs integer division (floor division).

**Parameters:**
- `a` (Number) - Dividend
- `b` (Number) - Divisor

**Returns:** `Number` - Integer quotient

**Example:**
```ez
result = floorDiv(17, 5)  // 3
result2 = floorDiv(20, 4) // 5
```

---

## Usage Examples

### Example 1: Age Calculator

```ez
load "datetime.ez"

// Birth date: May 15, 1990
birth = makeTimestamp(1990, 5, 15, 0, 0, 0)
today = now()

// Calculate age in days
age_days = diffDays(today, birth)
age_years = floorDiv(age_days, 365)

say "You are approximately " + str(age_years) + " years old"
say "That's " + str(age_days) + " days!"
```

### Example 2: Countdown Timer

```ez
load "datetime.ez"

// Event date: December 31, 2024 at midnight
event = makeTimestamp(2024, 12, 31, 0, 0, 0)
current = now()

days_until = diffDays(event, current)

say "Countdown to New Year 2025:"
say str(days_until) + " days remaining"
say "Event date: " + format(event)
```

### Example 3: Meeting Scheduler

```ez
load "datetime.ez"

// Schedule a meeting for tomorrow at 2 PM
meeting_time = addTime(now(), 1, 0, 0, 0)  // Add 1 day
components = getComponents(meeting_time)

// Set specific time to 14:00:00
meeting = makeTimestamp(
    components.year,
    components.month,
    components.day,
    14,  // 2 PM
    0,
    0
)

say "Meeting scheduled for: " + format(meeting)
```

### Example 4: Working with Weekdays

```ez
load "datetime.ez"

weekday_names = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]

current = now()
components = getComponents(current)

say "Today is " + weekday_names[components.weekday]
```

### Example 5: Time Zone Offset (Manual)

```ez
load "datetime.ez"

// Current UTC time
utc = now()

// Convert to EST (UTC-5)
est_offset_hours = -5
est = addTime(utc, 0, est_offset_hours, 0, 0)

say "UTC time: " + format(utc)
say "EST time: " + format(est)
```

### Example 6: Date Validation

```ez
load "datetime.ez"

task isValidDate(year, month, day) {
    when month < 1 or month > 12 {
        give false
    }
    
    when day < 1 {
        give false
    }
    
    max_days = daysInMonth(year, month)
    when day > max_days {
        give false
    }
    
    give true
}

// Test
when isValidDate(2024, 2, 29) {
    say "2024-02-29 is valid"  // Leap year
}

when isValidDate(2023, 2, 29) {
    say "2023-02-29 is valid"  // Won't print - invalid
} else {
    say "2023-02-29 is invalid"
}
```

### Example 7: Duration Calculator

```ez
load "datetime.ez"

start = makeTimestamp(2024, 1, 1, 9, 0, 0)
end = makeTimestamp(2024, 1, 1, 17, 30, 0)

diff_ms = diffMs(end, start)
hours = floorDiv(diff_ms, MS_PER_HOUR)
remaining_ms = diff_ms % MS_PER_HOUR
minutes = floorDiv(remaining_ms, MS_PER_MIN)

say "Work duration: " + str(hours) + " hours and " + str(minutes) + " minutes"
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

### Development Guidelines

1. Maintain compatibility with EZ language syntax
2. Include examples for new functions
3. Update documentation when adding features
4. Follow the existing code style and naming conventions

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Changelog

### Version 2.0.0
- Ported to new EZ interpreter
- Improved performance and accuracy
- Enhanced documentation
- Added helper functions

### Version 1.0.0
- Initial release
- Core datetime functionality
- Basic formatting and arithmetic operations

## Support

For bugs, questions, or suggestions, please open an issue on the GitHub repository.

---

**Made with ❤️ for the EZ Language Community**
