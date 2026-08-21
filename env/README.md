# env — .env and system environment loading for EZ

> **Version:** 1.1.0

---

## Overview

`env` loads `.env` files into your EZ scripts, and gives you direct,
cross-platform access to the host system environment too. A `.env` file is a
plain text file of `KEY=value` pairs, one per line, that keeps configuration
— database URLs, API keys, debug flags — out of source code.

- **Parse** `.env` files with full quoting and escaping support
- **Typed accessors** — `getInt`, `getFloat`, `getBool`, `getList`
- **Required keys** — declare what must exist; get a clear error if it doesn't
- **Multi-file merge** — layer `.env`, `.env.local`, `.env.production`
- **Host system environment** — read and write real OS process environment
  variables on Windows, Linux, and macOS
- **Structured errors** — `ParseError`, `KeyError`, `TypeError`, all with
  line numbers and filenames

---

## Quick Start

```ez
use "env"

env = load()                              # loads .env from current directory
port = env.getInt("PORT", 8080)
debug = env.getBool("DEBUG", false)
dbUrl = env.get("DATABASE_URL")

out "Starting on port " + str(port)
```

### Example `.env` file

```
# Database
DATABASE_URL=postgres://localhost:5432/myapp
DB_POOL_SIZE=10

# Server
PORT=3000
HOST=0.0.0.0
DEBUG=true

# API keys
API_KEY="sk-abc123def456"
SECRET='keep-it-literal'

# Comma-separated list
ALLOWED_ORIGINS=http://localhost:3000,https://myapp.com
```

---

## Loading

### `load(path = ".env", options = nil)` → `Env`

Load a `.env` file from disk.

```ez
env = load()                                    # .env
env = load(".env.production")                   # specific file
env = load(".env", { "required": ["DB_HOST"] }) # with validation
env = load(".env", { "defaults": { "PORT": "8080" } })
```

**Options:**

| Key | Type | Description |
|-----|------|-------------|
| `system` | bool | If `true`, host/server environment variables take precedence over `.env` file values. |
| `populate` | bool | If `true`, exports loaded `.env` variables into the host OS process environment (`setenv`). |
| `required` | array | Keys that must be present (checked against `.env` and host environment). Throws `KeyError` on miss. |
| `defaults` | dict | Fallback values for keys not found in the file. File values always win. |

### `load_string(content, options = nil)` → `Env`

Parse from a string instead of a file. Useful for testing or when `.env` data
arrives from a network response. Accepts the same options as `load()`.

```ez
env = load_string("PORT=3000\nDEBUG=true")
```

### `load_files(paths, priority = "first", options = nil)` → `Env`

Load multiple files and merge them. Missing files are silently skipped.

```ez
# First file wins (default) — .env is the base, .env.local fills in gaps
env = load_files([".env", ".env.local"])

# Last file wins — .env.production overrides everything
env = load_files([".env", ".env.production"], "last")
```

---

## The `Env` Model

Every `load*` function returns an `Env` instance.

### Core Accessors

| Method | Returns | Description |
|--------|---------|-------------|
| `env.get(key, fallback = nil)` | string or fallback | Get a value. Checks host system environment first if `system: true`. |
| `env.set(key, value)` | `self` | Set a value (in memory only, never writes to disk). |
| `env.has(key)` | bool | Check if a key exists in `.env` or (if `system: true`) the host environment. |
| `env.populate(overwrite = false)` | `self` | Write all in-memory variables into the host OS process environment. |
| `env.remove(key)` | `self` | Delete a key. |
| `env.all()` | dict | Return all key-value pairs as a dictionary. |
| `env.keys()` | array | Return all keys. |
| `env.size()` | int | Number of variables loaded. |

### Typed Accessors

Every value in a `.env` file is a string. These helpers coerce to the
requested type and throw `TypeError` if the value cannot be converted.

| Method | Returns | Recognised values |
|--------|---------|-------------------|
| `env.getInt(key, fallback)` | integer | Any string `num()` can parse as an integer |
| `env.getFloat(key, fallback)` | float | Any string `num()` can parse |
| `env.getBool(key, fallback)` | bool | `true`/`1`/`yes`/`on` → true; `false`/`0`/`no`/`off`/`""` → false |
| `env.getList(key, delimiter, fallback)` | array | Splits on `delimiter` (default `","`) and trims each element |

```ez
use "env"
env = load()

port    = env.getInt("PORT", 8080)
debug   = env.getBool("DEBUG", false)
pi      = env.getFloat("PRECISION", 3.14)
hosts   = env.getList("ALLOWED_HOSTS")         # ["a", "b", "c"]
origins = env.getList("CORS", ";")             # custom delimiter
```

### Required Keys

```ez
env = load()
env.require(["DATABASE_URL", "SECRET_KEY", "API_KEY"])
env.validate()     # throws KeyError on the first missing key
```

Or inline at load time:

```ez
env = load(".env", { "required": ["DATABASE_URL", "SECRET_KEY"] })
```

### Merge

```ez
base = load(".env")
overrides = load_string("PORT=9000\nNEW_KEY=hello")

base.merge(overrides, false)    # new keys added, existing keys kept
base.merge(overrides, true)     # new keys added, existing keys overwritten
```

`merge` also accepts a plain dictionary in place of an `Env` instance.

---

## Host System Environment

`env` isn't limited to `.env` files — it can talk to the real OS process
environment directly, on Windows, Linux, and macOS.

### `sys(key, fallback = nil)`

Read a variable straight from the host system environment, no `Env` instance
required.

```ez
use "env"

token = sys("K8S_SECRET_TOKEN")
region = sys("AWS_REGION", "us-east-1")
```

### `system` option

Pass `{ "system": true }` to any `load*` function to have host environment
variables take precedence over `.env` file values on `get()` and `has()` —
handy for letting a real deployment environment override local config.

```ez
env = load(".env", { "system": true })
port = env.get("PORT")   # host PORT wins if set, otherwise falls back to .env
```

### `populate` option / `env.populate()`

Push variables the other direction — write everything loaded from `.env`
into the host OS process environment, so subsequently-spawned processes or
other libraries reading `getenv()` can see them.

```ez
env = load(".env", { "populate": true })

# or manually:
env = load()
env.populate(true)   # true = overwrite existing host variables
```

---

## `.env` Syntax

| Syntax | Meaning |
|--------|---------|
| `KEY=value` | Simple key-value pair |
| `KEY = value` | Spaces around `=` are stripped |
| `KEY="value"` | Double-quoted: `\n` `\t` `\\` `\"` are unescaped |
| `KEY='value'` | Single-quoted: taken verbatim, no escaping |
| `` KEY=`value` `` | Backtick-quoted: taken verbatim |
| `export KEY=value` | `export` prefix ignored (bash compatibility) |
| `# comment` | Full-line comment |
| `KEY=value # comment` | Inline comment (unquoted values only) |
| `KEY=` | Empty value (empty string, not nil) |

**Rules:**
- Keys must contain only `A-Z`, `a-z`, `0-9`, `_`, `.` and must not start with a digit.
- Duplicate keys: **last value wins**.
- Lines without an `=` sign are silently skipped (docker-compose compatibility).
- `#` inside quotes is a literal character, not a comment.

---

## Errors

All errors are thrown as formatted strings with a structured prefix.

| Error | Thrown when |
|-------|------------|
| `ParseError` | Malformed `.env` syntax: empty key, invalid character in key, unterminated quote, unreadable file. Includes line number and filename. |
| `KeyError` | A required key is missing after `validate()`. |
| `TypeError` | A typed accessor (`getInt`, `getBool`, `getFloat`) cannot coerce the value. |

```ez
try {
    env = load(".env", { "required": ["MISSING_KEY"] })
} catch e {
    out e    # "ezenv: KeyError: required environment variable 'MISSING_KEY' is not set"
}
```

---

## Patterns

### Web app configuration

```ez
use "env"
use "web"

env = load(".env", { "required": ["SECRET_KEY", "DATABASE_URL"] })

app = WebApp()
app.config_set("secret", env.get("SECRET_KEY"))
app.config_set("debug", env.getBool("DEBUG", false))

app.run(env.getInt("PORT", 8080))
```

### Per-environment layering

```ez
use "env"

mode = "production"   # or read from argv
env = load_files([".env", ".env.local", ".env." + mode], "last")
```

### Letting the real deployment environment win

```ez
use "env"

# In production, host-set PORT/DATABASE_URL override .env defaults.
env = load(".env", { "system": true })
```

### Defaults for development

```ez
env = load(".env", {
    "defaults": {
        "PORT": "3000",
        "HOST": "127.0.0.1",
        "DEBUG": "true",
        "LOG_LEVEL": "debug"
    }
})
```

---

## Tests

```
cd C:\ezlib\env
ez test_env.ez
```

55+ assertions covering: basic parsing, spaces, comments, inline comments,
all three quoting styles, escape sequences, export prefix, empty values,
duplicate keys, typed accessors (int, float, bool, list), fallbacks,
set/remove/has, require/validate, merge, defaults, host system environment
(`sys()`, `system` option, `populate`), and error handling (ParseError,
KeyError, TypeError).

---

## License

MIT
