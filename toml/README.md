# toml — TOML configuration for EZ

> **Import:** `use "toml"`
> **Install:** `ez install toml`

```ez
use "toml"

config = TOML.load("config.toml")
out config["server"]["port"]        # 8080, as a number
```

---

## Quick Start

```toml
# config.toml
title = "My App"
port = 8080

[server]
host = "localhost"

[server.tls]
enabled = true

[[endpoints]]
path = "/health"

[[endpoints]]
path = "/metrics"
```

```ez
config = TOML.parse(readFile("config.toml"))

config["title"]                       # "My App"
config["port"]                        # 8080
config["server"]["tls"]["enabled"]    # true
config["endpoints"][0]["path"]        # "/health"

TOML.get(config, "server.tls.enabled", false)
TOML.get(config, "cache.ttl", 300)    # missing table → 300
```

`TOML.get` exists because reaching into a missing table is the usual way
config code breaks: `config["cache"]["ttl"]` throws when there is no
`[cache]`, and `TOML.get` cannot.

## Types are explicit

This is the main difference from INI, and the reason to choose TOML:

| Written | Parsed as |
| --- | --- |
| `port = 8080` | integer |
| `ratio = 1.5` | float |
| `debug = true` | bool |
| `name = "text"` | string |
| `path = 'C:\raw'` | string, no escapes applied |
| `when = 1979-05-27` | string (kept verbatim) |
| `xs = [1, 2, 3]` | array |
| `pt = { x = 1, y = 2 }` | dictionary |

**A bare unquoted word is an error, not a string.** `name = hello` throws.
That strictness is the point: there is no ambiguity about whether `1.10` is a
number or a version, because a version must be written `"1.10"`.

Underscores are allowed as digit separators: `1_000_000`.

Literal strings (`'...'`) apply no escapes at all, which is what makes them
right for Windows paths and regular expressions.

## Structure

**Nested tables** — something INI cannot express:

```toml
[a.b.c]
x = 1        # config["a"]["b"]["c"]["x"]
```

**Dotted keys** do the same inline:

```toml
a.b.c = 1
```

**Arrays of tables** with `[[name]]`, for repeated records:

```toml
[[user]]
name = "Ali"

[[user]]
name = "Sara"
```

A `[user.meta]` header after `[[user]]` applies to the most recently
appended entry.

## API

| Call | Result |
| --- | --- |
| `TOML.parse(source)` | Parse. Throws with a line number on bad input |
| `TOML.parseOr(source, fallback)` | Same, returning `fallback` instead |
| `TOML.load(path)` | Read a file and parse |
| `TOML.isValid(source)` | Whether it parses |
| `TOML.stringify(data)` | Serialise back to TOML |
| `TOML.save(path, data)` | Serialise and write |
| `TOML.get(data, "a.b", fallback)` | Read a path safely |
| `TOML.has(data, "a.b")` | Whether the path exists |

## Round-tripping

`TOML.parse(TOML.stringify(data))` returns an equal value.

Top-level keys are always emitted **before** any table header — anything
written after a `[section]` would be read back as part of it, which silently
reparents the value.

## Choosing a config format

| Use | When |
| --- | --- |
| `toml` | Nested config with real types, edited by hand |
| `ini` | Flat settings, simplest possible format |
| `yaml` | Deep nesting, lists, multi-line text |
| `json` | Machine-generated data, or interop |
| `env` | Secrets and per-environment values |

TOML's advantage over YAML is that it is not indentation-sensitive; over INI,
that it nests and has real types.

## Limits

- **No multi-line strings** (`"""..."""` / `'''...'''`).
- **Dates and times are kept as strings.** EZ has no date type here, and
  turning one into a number would lose it. Parse with `datetime` if needed.
- **No comment or ordering preservation.** Rewriting a hand-edited file with
  `stringify` reformats it and drops comments.
- **No offset date-time validation** — anything date-shaped is kept verbatim.
- Duplicate keys are not rejected; the last one wins.

## Testing

```
ez test.ez
```

71 tests covering every scalar type, both string forms, dotted keys and
headers, arrays, inline tables, arrays of tables, sub-tables of array
entries, round-tripping and the errors that make TOML strict — bare words,
unterminated strings and malformed headers.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `TOML` model: parsing entry points, access, serialisation |
| `parse.ez` | Document structure: headers, key paths, arrays, inline tables |
| `value.ez` | Scalar typing and string escapes |

## License

MIT
