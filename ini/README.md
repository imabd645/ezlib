# ini — INI configuration files for EZ

> **Import:** `use "ini"`
> **Install:** `ez install ini`

Read and write INI files, with values typed the way they are written.

```ez
use "ini"

config = INI.load("app.ini")
out config["database"]["port"]        # 5432, as a number
```

---

## Quick Start

```ini
; app.ini
name = my-app
debug = true

[database]
host = localhost
port = 5432
password = "p@ss;word"
```

```ez
use "ini"

config = INI.parse(readFile("app.ini"))

config["name"]                        # "my-app"
config["debug"]                       # true  (a bool)
config["database"]["port"]            # 5432  (a number)

INI.get(config, "database.host", "localhost")
INI.get(config, "cache.ttl", 300)     # section missing -> 300
```

Keys written before any `[section]` land at the top level. Keys after one
belong to that section.

## API

| Call | Result |
| --- | --- |
| `INI.parse(source)` | `{ section: { key: value } }`. Throws on malformed input |
| `INI.parseOr(source, fallback)` | Same, returning `fallback` instead of throwing |
| `INI.load(path)` | Read a file and parse it |
| `INI.isValid(source)` | Whether it parses |
| `INI.stringify(data)` | Serialise back to INI text |
| `INI.save(path, data)` | Serialise and write |
| `INI.get(data, "a.b", fallback)` | Read a path without risking a nil index |
| `INI.has(data, "a.b")` | Whether that path exists |
| `INI.sections(data)` | Section names, sorted |

`INI.get` exists because reaching into a missing section is the usual way INI
code breaks. `config["cache"]["ttl"]` throws when there is no `[cache]`;
`INI.get(config, "cache.ttl", 300)` cannot.

## How values are typed

| Written | Parsed as | `typeOf` |
| --- | --- | --- |
| `port = 5432` | `5432` | `integer` |
| `ratio = 1.5` | `1.5` | `float` |
| `debug = true` / `yes` / `on` | `true` | `bool` |
| `debug = false` / `no` / `off` | `false` | `bool` |
| `value = null` / `nil` / `none` | `nil` | `nil` |
| `name = my-app` | `"my-app"` | `string` |
| `port = "5432"` | `"5432"` | `string` |

Booleans and nulls are case-insensitive. Anything that is not a number,
boolean or null is a string.

> **Quote to keep something a string.** `version = 1.10` is the number `1.1`.
> Write `version = "1.10"`. The same applies to zip codes, phone numbers and
> IDs with leading zeroes. Note `1.2.3` is already a string — it is not a
> number — so only two-part values are at risk.

## Syntax accepted

**Separators** — both `=` and `:` work: `key = value`, `key: value`.

**Comments** — `;` or `#` to end of line, when preceded by whitespace or at
line start. `pass = a#b` keeps its hash, because an unquoted password
containing one is common enough that eating it would be worse.

**Quoting** — double quotes support `\n`, `\t`, `\"` and `\\`; single quotes
are literal. Quotes preserve leading and trailing spaces.

**Sections** — `[name]`, with surrounding spaces trimmed. A repeated section
merges into the existing one rather than replacing it.

**Blank lines** are ignored.

## Round-tripping

`INI.parse(INI.stringify(data))` returns an equal value. Strings that would
otherwise be re-read as another type are quoted automatically:

```ez
INI.stringify({ "a": "true", "b": "42" })
```

```ini
a = "true"
b = "42"
```

Top-level keys are always emitted before any section, since anything written
after a `[section]` header would be read back as part of it.

## Limits

- **Order is not preserved.** Sections and keys come back in unspecified order
  (`INI.sections` sorts, for determinism). Comments are discarded. Rewriting a
  hand-edited file with `stringify` will reformat it.
- No nested sections (`[a.b]` is a section literally named `a.b`).
- No multi-line values or line continuations.
- No duplicate-key handling — the last one wins.

If you need comments and ordering preserved, edit the file as text rather than
round-tripping it through this library.

## Choosing a config format

| Use | When |
| --- | --- |
| `ini` | Flat, human-edited settings. Simplest to read and write by hand |
| `yaml` | Nesting, lists, multi-line text |
| `json` | Machine-generated data, or interop |
| `env` | Secrets and per-environment values |

## Testing

```
ez test.ez
```

58 tests covering typing, quoting, comments, sections, access helpers,
round-tripping and error reporting.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `INI` model and access helpers |
| `parse.ez` | Parsing, value typing, and serialisation |

## License

MIT
