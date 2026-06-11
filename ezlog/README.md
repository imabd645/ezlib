# ezlog — Logging Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezlog"`  
> **File:** `E:\ezlib\ezlog\main.ez`  
> **Depends on:** `ezfs`

---

## Overview

`ezlog` provides a structured, level-filtered, colorized logging system for EZ. It supports five severity levels, optional file output, colored terminal output, and time-prefixed messages.

After import, a ready-to-use **singleton** `log` instance is available immediately.

---

## Quick Start

```ez
use "ezlog"

log.info("Application started")
log.warn("Config file not found, using defaults")
log.error("Database connection failed")
```

**Output:**
```
[08:30:00.123] [INFO] Application started
[08:30:00.124] [WARN] Config file not found, using defaults
[08:30:00.125] [ERROR] Database connection failed
```

---

## Level Constants

| Constant | Value | Color | Description |
|---|---|---|---|
| `DEBUG` | `0` | Gray (8) | Verbose debugging information |
| `INFO` | `1` | Green (10) | Normal operational messages |
| `WARN` | `2` | Yellow (14) | Non-fatal warnings |
| `ERROR` | `3` | Red (12) | Error conditions |
| `FATAL` | `4` | White on Red (79) | Critical failures |

These constants are exported as globals when you `use "ezlog"`.

---

## The `log` Singleton

After `use "ezlog"`, a pre-configured `Logger` instance named `log` is available:

```ez
use "ezlog"

log.info("Ready")   # Default level = INFO, console = true, file = none
```

---

## Model: `Logger`

### `Logger()`

Creates a new Logger instance with default settings.

```ez
use "ezlog"

myLogger = Logger()
myLogger.setLevel(DEBUG)
myLogger.info("Custom logger ready")
```

---

## Configuration Methods

### `logger.setLevel(level)`

Sets the minimum severity level. Messages below this level are silently ignored.

**Parameters:**
- `level` — One of `DEBUG`, `INFO`, `WARN`, `ERROR`, or `FATAL` (integers 0–4).

```ez
use "ezlog"

log.setLevel(DEBUG)   # Show everything
log.setLevel(WARN)    # Only show WARN and above
log.setLevel(ERROR)   # Only ERROR and FATAL
```

---

### `logger.setFile(path)`

Sets a file path for log output. All messages (at or above the current level) will also be appended to this file.

**Parameters:**
- `path` — Path to the log file. The file will be created if it doesn't exist.

```ez
use "ezlog"

log.setFile("app.log")
log.info("This goes to console AND app.log")
```

---

### `logger.enableConsole(bool)`

Enables or disables console (stdout) output.

```ez
use "ezlog"

log.enableConsole(false)   # Silent mode — only write to file
log.setFile("silent.log")
log.error("This only goes to file, not console")
```

---

## Logging Methods

### `logger.debug(msg)`

Logs a message at DEBUG level (gray). Only shown when `setLevel(DEBUG)` is active.

```ez
use "ezlog"

log.setLevel(DEBUG)
log.debug("Processing item 42")
log.debug("Cache miss for key: user_123")
```

---

### `logger.info(msg)`

Logs a message at INFO level (green). This is the default minimum level.

```ez
use "ezlog"

log.info("Server started on port 8080")
log.info("User logged in: alice@example.com")
```

---

### `logger.warn(msg)`

Logs a message at WARN level (yellow).

```ez
use "ezlog"

log.warn("Disk usage above 80%")
log.warn("Deprecated API endpoint called: /v1/users")
```

---

### `logger.error(msg)`

Logs a message at ERROR level (red).

```ez
use "ezlog"

log.error("Failed to connect to database")
log.error("File not found: config.json")
```

---

### `logger.fatal(msg)`

Logs a message at FATAL level (white on red background).

```ez
use "ezlog"

log.fatal("Out of memory — system cannot continue")
log.fatal("Corrupt data detected in critical table")
```

---

## Output Format

Every log line follows this format:
```
[HH:MM:SS.mmm] [LEVEL] message
```

- **HH:MM:SS.mmm** — System uptime formatted as hours:minutes:seconds.milliseconds (based on `clock()`)
- **[LEVEL]** — `DEBUG`, `INFO`, `WARN`, `ERROR`, or `FATAL`
- **message** — The string passed to the log method

**Example output:**
```
[00:05:32.847] [INFO] Server started
[00:05:33.001] [WARN] Memory at 78%
[00:05:33.012] [ERROR] Connection refused: db.prod:5432
```

> ⚠️ **Note:** The time shown is relative to system uptime (from `clock()`), not the wall-clock time. To display wall-clock time, integrate `ezdatetime`.

---

## Edge Cases & Important Notes

### Time Format
The `_formatTime()` method uses `clock()` which returns milliseconds since program start (or system boot, depending on EZ version). It is **not** the real wall clock time.

### File Path Must Be Set Before Logging
Setting `setFile()` after some logs have already been written means those early logs won't be in the file:
```ez
use "ezlog"

log.info("This won't be in the file")  # Before setFile
log.setFile("app.log")
log.info("This WILL be in the file")   # After setFile
```

### Log File is Append-Only
The file logger uses `fs.appendFile()`. Each run of the program appends to the existing log file. To start fresh, delete or truncate the file before starting.

### Console Colors Use Windows Console API
Colors are applied using the EZ built-in `color(code)` function which calls Windows Console API. This only works in Windows terminal emulators that support it (cmd.exe, PowerShell, Windows Terminal). In non-Windows environments, the `color()` and `reset()` calls are no-ops.

### Concurrent Logging
`ezlog` is not thread-safe. In multi-threaded EZ programs using `spawn`, log writes from different threads may interleave. Consider a dedicated logging thread or wrapping log calls in a mutex.

### Large Messages
Very long messages are logged as-is (no truncation). Extremely large objects stringified via `str()` may produce verbose output.

---

## Full Example: Multi-Level Application Logger

```ez
use "ezlog"

# Create dedicated loggers for different components
appLogger = Logger()
appLogger.setLevel(INFO)
appLogger.setFile("application.log")

dbLogger = Logger()
dbLogger.setLevel(DEBUG)
dbLogger.setFile("database.log")

httpLogger = Logger()
httpLogger.setLevel(WARN)
httpLogger.setFile("http.log")

# Simulate application lifecycle
appLogger.info("=== Application Starting ===")

dbLogger.debug("Connecting to database at localhost:5432")
dbLogger.info("Database connection established")

httpLogger.info("HTTP server binding to 0.0.0.0:8080")

appLogger.info("All systems ready. Accepting requests.")

# Simulate some events
httpLogger.warn("Rate limit reached for IP: 192.168.1.100")
dbLogger.warn("Query took 2345ms (threshold: 1000ms)")
appLogger.error("Unhandled exception in route /api/orders")

appLogger.info("=== Application Shutting Down ===")
```

---

## Full Example: Structured Request Logger

```ez
use "ezlog"

log.setLevel(DEBUG)
log.setFile("requests.log")

task logRequest(method, path, status, durationMs) {
    msg = method + " " + path + " -> " + str(status) + " (" + str(durationMs) + "ms)"
    
    when status >= 500 {
        log.error(msg)
    } other when status >= 400 {
        log.warn(msg)
    } other {
        log.info(msg)
    }
}

# Simulate HTTP requests
logRequest("GET", "/api/users", 200, 45)
logRequest("POST", "/api/orders", 201, 123)
logRequest("GET", "/api/missing", 404, 12)
logRequest("POST", "/api/data", 500, 2341)
logRequest("GET", "/health", 200, 3)
```

---

## Full Example: Debug-Level Application Trace

```ez
use "ezlog"

log.setLevel(DEBUG)

task processOrder(orderId) {
    log.debug("Processing order #" + str(orderId))
    
    # Simulate steps
    log.debug("  → Validating inventory")
    log.debug("  → Calculating totals")
    log.debug("  → Charging payment method")
    
    when orderId % 5 == 0 {
        log.error("  ✗ Payment declined for order #" + str(orderId))
        give false
    }
    
    log.info("Order #" + str(orderId) + " completed successfully")
    give true
}

repeat i = 1 to 10 {
    processOrder(i)
}
```

---

## Custom Logger Example

```ez
use "ezlog"

# Create a logger that only writes to file (no console spam in production)
prodLogger = Logger()
prodLogger.setLevel(WARN)
prodLogger.setFile("production.log")
prodLogger.enableConsole(false)

prodLogger.debug("This is ignored (below WARN level)")
prodLogger.info("This is also ignored")
prodLogger.warn("This goes to production.log only")
prodLogger.error("This goes to production.log only")
prodLogger.fatal("CRITICAL: This goes to production.log only")
```

---

*Documentation generated from `E:\ezlib\ezlog\main.ez` — EZ Logging Library*
