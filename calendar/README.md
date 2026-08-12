# Calendar Library

The `calendar` library provides functions to display well-formatted calendars on the console in EZ. It utilizes the native `DateTime` class.

## Installation

Since this library is included in the standard `ezlib` packages, you can import it directly:
```ez
use "calendar"
```

## Functions

### `printMonth(year, month)`
Displays a calendar grid for a specific month and year.
```ez
calendar.printMonth(2024, 2)
```

### `printYear(year)`
Displays a full 12-month calendar for a given year.
```ez
calendar.printYear(2026)
```

### `printCurrentMonth()`
Displays the calendar grid for the current system month.
```ez
calendar.printCurrentMonth()
```

### `printCurrentYear()`
Displays the full 12-month calendar for the current system year.
```ez
calendar.printCurrentYear()
```

## Example Usage

```ez
use "calendar"

out "=== This Month ==="
calendar.printCurrentMonth()

out ""
out "=== 2024 Leap Year ==="
calendar.printMonth(2024, 2)
```
