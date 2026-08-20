# url

URL parsing, RFC 3986 percent-encoding, and query-string handling. Pure EZ, no
dependencies.

```ez
use "url"

urlParse("https://u:p@example.com:8080/a?q=1#f")["host"]   # "example.com"
urlEncodeComponent("a b&c")     # "a%20b%26c"
urlDecodeComponent("a%20b")     # "a b"
urlBuildQuery({ "a": 1, "b": "x y" })   # "a=1&b=x%20y"
urlParseQuery("a=1&b=x+y")      # { a: "1", b: "x y" }
```

## API

| Function | Returns |
|----------|---------|
| `urlParse(url)` | `{ scheme, userinfo, host, port, path, query, fragment }` |
| `urlEncodeComponent(s)` | percent-encoded string |
| `urlDecodeComponent(s)` | decoded string |
| `urlBuildQuery(params)` | query string |
| `urlParseQuery(qs)` | dictionary |

## Encoding

`urlEncodeComponent` follows RFC 3986: only `A-Z a-z 0-9 - _ . ~` stay literal,
everything else becomes `%XX` of its bytes. UTF-8 text therefore encodes to two
hex escapes per character (`é` → `%C3%A9`), which is what a server expects.

`urlDecodeComponent` decodes `%XX` and also treats `+` as a space, so the same
function works for form-encoded query strings as for path segments.

## Parsing

`urlParse` splits a URL into named parts. Parts that are absent are `nil`, not
empty strings:

| URL | scheme | host | port | path | query | fragment |
|-----|--------|------|------|------|-------|----------|
| `https://h/a?q#f` | `https` | `h` | `nil` | `/a` | `q` | `f` |
| `http://example.com` | `http` | `example.com` | `nil` | `/` | `nil` | `nil` |
| `/relative?x=1` | `nil` | `nil` | `nil` | `/relative` | `x=1` | `nil` |
| `mailto:a@b.c` | `mailto` | `nil` | `nil` | `a@b.c` | `nil` | `nil` |

A host with no explicit path gets `/` — the same normalisation Go's `net/url`
makes. IPv6 hosts are kept in brackets.

## Query strings

- `urlBuildQuery` sorts keys (deterministic output) and repeats a list value
  once per element.
- `urlParseQuery` collects a repeated key into a list (`a=1&a=2` →
  `{ a: ["1", "2"] }`), and treats a bare `flag` as `{ flag: "" }`.
