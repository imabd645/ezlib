# env — .env file loading for EZ

> **Version:** 1.0.0  
> **Import:** `use "env"`  
> **File:** `C:\ezlib\env\main.ez`  
> **Requires:** nothing — pure EZ, no FFI, no DLLs

---

## Overview

`env` loads `.env` files into your EZ scripts. A `.env` file is a plain text
file of `KEY=value` pairs, one per line, that keeps configuration — database
URLs, API keys, debug flags — out of source code.

- **Parse** `.env` files with full quoting and escaping support
- **Typed accessors** — `getInt`, `getFloat`, `getBool`, `getList`
- **Required keys** — declare what must exist; get a clear error if it doesn't
- **Multi-file merge** — layer `.env`, `.env.local`, `.env.production`
- **Structured errors** — `ParseError`, `KeyError`, `TypeError`, all with
  line numbers and filenames

The library is modular, following the same architecture as `sqlite`:

```
env/
  package.ez         ← manifest
  main.ez            ← public surface (load, load_string, load_files)
  src/
    errors.ez        ← structured error builders
    parser.ez        ← the only file that reads raw .env content
    store.ez         ← Env model: get/set/has/require/typed accessors
  test_env.ez        ← test suite
```

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
| `required` | array | Keys that must be present. Throws `KeyError` on the first miss. |
| `defaults` | dict | Fallback values for keys not found in the file. File values always win. |

### `load_string(content, options = nil)` → `Env`

Parse from a string instead of a file. Useful for testing or when `.env` data
arrives from a network response.

```ez
env = load_string("PORT=3000\nDEBUG=true")
```

### `load_files(paths, priority = "first")` → `Env`

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
| `env.get(key, fallback = nil)` | string or fallback | Get a value. Returns `fallback` if the key is not set. |
| `env.set(key, value)` | `self` | Set a value (in memory only, never writes to disk). |
| `env.has(key)` | bool | Check if a key exists. |
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
    out e    # "env: KeyError: required environment variable 'MISSING_KEY' is not set"
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

```
.env                ← base config, committed to git
.env.local          ← local overrides, gitignored
.env.production     ← production secrets, gitignored
```

```ez
use "env"

mode = "production"   # or read from argv
env = load_files([".env", ".env.local", ".env." + mode], "last")
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
set/remove/has, require/validate, merge, defaults, and error handling
(ParseError, KeyError, TypeError).

---

## Architecture

Following the same modular pattern as `sqlite`:

| File | Responsibility |
|------|----------------|
| `main.ez` | Public surface — `load()`, `load_string()`, `load_files()`. No logic. |
| `src/parser.ez` | The only file that reads raw `.env` content. Handles all quoting, escaping, comments, and key validation. |
| `src/store.ez` | The `Env` model. All runtime API: accessors, typed coercion, require/validate, merge. |
| `src/errors.ez` | Structured error builders. Every failure is diagnosable by type. |

---

## Not covered

- **Process environment variables**: `env` does not read or write the OS
  process environment (`getenv`/`setenv`). It is an in-memory store seeded
  from files.
- **Variable interpolation**: `DB_URL=${DB_HOST}:${DB_PORT}` is not expanded.
  This is a deliberate omission — interpolation interacts badly with quoting
  and with values that contain `$` literally (API keys).
- **Multi-line values**: A value cannot span multiple lines, even when quoted.
  Use `\n` inside double quotes instead.

---

## License

MIT
