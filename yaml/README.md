# yaml — YAML for EZ

> **Import:** `use "yaml"`
> **Install:** `ez install yaml`

A YAML parser and serialiser written in pure EZ. No native dependencies, no
FFI — it works anywhere EZ does.

Reading configuration is the job YAML is used for most, so the library is
built around getting that exactly right: types are inferred the way YAML
specifies, and `stringify` round-trips — parsing what it writes gives back an
equal value.

```ez
use "yaml"

config = YAML.load("config.yaml")
out config["server"]["port"]        # 8080, as a number

config["server"]["port"] = 9090
YAML.save("config.yaml", config)
```

---

## Quick Start

```ez
use "yaml"

source = "name: my-app
version: 1.4
debug: true
tags: [web, api]
server:
  host: localhost
  port: 8080"

config = YAML.parse(source)

out config["name"]              # my-app
out config["version"]           # 1.4
out config["server"]["port"]    # 8080
out config["tags"][0]           # web
```

## API

| Call | Result |
| --- | --- |
| `YAML.parse(source)` | Parse a document into EZ values. Throws on malformed input. |
| `YAML.parseOr(source, fallback)` | Same, but returns `fallback` instead of throwing. |
| `YAML.stringify(value)` | Serialise EZ values to YAML text. |
| `YAML.load(path)` | Read a file and parse it. |
| `YAML.save(path, value)` | Serialise and write a file. Returns `true`. |
| `YAML.isValid(source)` | `true` when the source parses. |

## How values are typed

A scalar's spelling determines its type, which is why quoting matters.

| YAML | EZ value | `typeOf` |
| --- | --- | --- |
| `port: 8080` | `8080` | `integer` |
| `port: "8080"` | `"8080"` | `string` |
| `ratio: 1.5` | `1.5` | `float` |
| `debug: true` | `true` | `bool` |
| `debug: "true"` | `"true"` | `string` |
| `value: null` / `~` / *(empty)* | `nil` | `nil` |
| `name: my-app` | `"my-app"` | `string` |

`true`/`yes`/`on` and `false`/`no`/`off` are all booleans, in any capitalisation
YAML allows. Everything that is not a number, boolean or null is a string.

> **Quote anything you need to stay a string.** A version like `version: 1.10`
> is the number `1.1`. Write `version: "1.10"` to keep it intact. The same
> applies to zip codes, phone numbers and ports you intend to concatenate.

## Supported syntax

**Mappings and sequences**, nested by indentation:

```yaml
database:
  host: localhost
  ports:
    - 5432
    - 5433
```

**Sequences of mappings** — the common shape for lists of records:

```yaml
users:
  - name: Ali
    role: admin
  - name: Sara
    role: editor
```

**Flow collections**, inline and nestable:

```yaml
tags: [web, api, internal]
limits: {cpu: 2, memory: 512}
matrix: [[1, 2], [3, 4]]
```

**Block scalars** for multi-line text. `|` keeps newlines, `>` folds them into
spaces, and the chomping indicators control the ending:

```yaml
description: |
  First line.
  Second line.

summary: >
  This is one long
  paragraph when parsed.

exact: |-
  No trailing newline.
```

**Comments** — from `#` to end of line, ignored unless inside quotes:

```yaml
port: 8080     # the public port
url: http://example.com/#anchor   # not a comment
```

**Quoted strings** — double quotes support `\n`, `\t`, `\r`, `\0`, `\"`, `\\`
and `\/`; single quotes are literal, with `''` meaning one quote.

## Errors

Parse failures throw an `Exception` naming the line:

```ez
try {
    config = YAML.parse(source)
}
catch (e) {
    out e.message      # YAML parse error (line 7): expected 'key: value' but found 'oops'
}
```

Use `parseOr` or `isValid` when a malformed file should not stop the program:

```ez
config = YAML.parseOr(readFile("config.yaml"), { "port": 8080 })
```

Content that cannot be attached to the structure is an error rather than
being skipped — silently dropping a line the author wrote is worse than
refusing the file.

## Round-tripping

`stringify` is written so that `YAML.parse(YAML.stringify(v))` equals `v`.
Strings that would otherwise be re-read as another type are quoted
automatically:

```ez
YAML.stringify({ "a": "true", "b": "8080", "c": "has: colon" })
```

```yaml
a: "true"
b: "8080"
c: "has: colon"
```

Multi-line strings are written as block scalars with the chomping indicator
that reproduces the original ending exactly.

## Not supported

This is a practical subset, not a complete YAML 1.2 implementation. The
following are **not** handled, and a document using them will either throw or
parse differently than a full implementation would:

- Anchors and aliases (`&name`, `*name`) and merge keys (`<<`)
- Explicit tags (`!!str`, `!!int`, custom tags)
- Multiple documents in one file (`---` separators)
- Complex mapping keys (`? key`)
- Non-string mapping keys — keys are converted to strings
- Date and timestamp types (they parse as strings)

Tabs are rejected for indentation, which YAML also forbids, with a message
saying so rather than a confusing structural error.

## Testing

```
ez test.ez
```

49 tests covering scalar typing, comments, nesting, flow collections, block
scalars, round-tripping and error reporting.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `YAML` model — the only import consumers need |
| `scanner.ez` | Source text into line records; comment stripping |
| `scalar.ez` | Scalar text into typed values; escape decoding |
| `parser.ez` | Recursive descent over lines; flow and block parsing |
| `emitter.ez` | EZ values back into YAML text |

## License

MIT
