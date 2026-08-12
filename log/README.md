# log

Logging for EZ: levels, console colour, file output with rotation, structured
fields, and JSON records.

All file access goes through the interpreter's builtin `File` class — the
package has no dependencies.

```ez
use "log"

info("server started")
warn("slow query", {"ms": 812, "table": "users"})
```

## Levels

`TRACE` `DEBUG` `INFO` `WARN` `ERROR` `FATAL` `OFF`

They are numbers spaced by ten (TRACE = 10 … FATAL = 60), so you can define your
own in between:

```ez
NOTICE = 35
log.log(NOTICE, "provisioned")
```

| Function | Result |
| --- | --- |
| `levelName(level)` | `"INFO"`; unknown levels render as `LVL35` |
| `levelFromName(name)` | level number, or `nil` if unrecognised. Case-insensitive, accepts `WARNING` |

## A logger

```ez
app = Logger("api")
app.setLevel(DEBUG)
app.toFile("api.log", 5 * 1024 * 1024, 5)   # rotate at 5 MB, keep 5 old files
app.info("listening", {"port": 8080})
```

Configuration methods return the logger, so they chain:

```ez
Logger("worker").setLevel(WARN).setFormat("json").toFile("worker.log")
```

### Configuration

| Method | Purpose |
| --- | --- |
| `setLevel(l)` | Minimum level. Accepts a number or a name (`"debug"`), since levels usually arrive from config as strings |
| `getLevel()` / `isEnabled(level)` | Current level; whether a level would be emitted |
| `toConsole(on)` / `setConsoleLevel(l)` / `setColor(on)` | Console sink |
| `toFile(path, maxBytes=0, backupCount=0)` | File sink. `maxBytes = 0` disables rotation |
| `setFileLevel(l)` | Let the file record more than the console (a common pairing: `DEBUG` to file, `INFO` to console) |
| `setFormat("text" \| "json")` | Output shape |
| `setTimeFormat(fmt)` / `showTimestamps(on)` | strftime pattern, default `%Y-%m-%d %H:%M:%S` plus milliseconds |
| `setContext(fields)` / `addContext(k, v)` | Fields attached to every record |

### Emitting

```ez
app.trace(msg, fields)   app.debug(msg, fields)   app.info(msg, fields)
app.warn(msg, fields)    app.error(msg, fields)   app.fatal(msg, fields)
app.log(level, msg, fields)
app.exception("upload failed", err, fields)
```

`fields` is optional. Values are rendered sorted and space-safe, so lines with
the same fields always look alike and stay greppable:

```
[2026-08-07 01:32:51.812] [WARN] [api] slow query ms=812 note="has spaces" table=users
```

### Structured output

`setFormat("json")` writes one object per line for log shippers:

```json
{"time":"2026-08-07 01:32:51.812","level":"WARN","logger":"api","msg":"slow query","ms":812}
```

### Per-request context

`with(fields)` returns a logger sharing this one's sinks and settings but
carrying extra fields — the usual way to tag every line of one request:

```ez
rlog = app.with({"req": requestId})
rlog.info("received")        # ... req=abc
rlog.info("responded")       # ... req=abc
```

`child("auth")` does the same and namespaces the logger: `api` → `api.auth`.

Loggers made this way **borrow** the parent's file handle rather than opening
the file twice, so closing a child leaves the parent writing.

### Rotation

With `toFile(path, maxBytes, backupCount)`, once a write would push the file
past `maxBytes` the logger renames `path` → `path.1`, `path.1` → `path.2`, and
so on, discarding anything past `backupCount`, then continues in a fresh file.

### Maintenance

| Method | Purpose |
| --- | --- |
| `flush()` | Push buffered writes to disk. Done automatically after each line unless you set `autoFlush = false` |
| `rotate()` | Rotate now, regardless of size |
| `close()` | Close the file sink |
| `lastError` | Message from the most recent sink failure, `""` if none |

## Failures never propagate

A logging call will not throw. If the file cannot be opened or written, the
reason is recorded in `lastError` and reported once on the console, and the
program carries on — losing a log line should not take an application down.

```ez
bad = Logger("bad")
bad.toFile("no_such_dir/x.log")   # does not throw
bad.info("still runs")            # console still works
out bad.lastError                 # "could not open log file '...': ..."
```

## The default logger

For the common single-logger case, module-level shortcuts write to a built-in
logger named `app`:

```ez
setLevel(DEBUG)
toFile("app.log", 1024 * 1024, 3)
info("ready")
error("failed", {"code": 500})
```

`getLogger(name)` returns a registry-backed logger, so the same name always
gives the same instance:

```ez
getLogger("db") == getLogger("db")   # true
```

## Tests

```
ez log/test_log.ez
```
