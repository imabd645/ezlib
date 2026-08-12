# regex

Regular expressions for EZ: compiled patterns, real match positions, capture
groups, flags, split/replace, plus validators and extractors.

```ez
use "regex"

when reTest(input, "^[0-9]+$") { out "all digits" }

p = Regex("(\\w+)@(\\w+)\\.com", "i")
m = p.find("mail Bob@Example.com now")
m.group(1)     # "Bob"
m.start        # 5
```

## Layout

| File | Contents |
| --- | --- |
| `main.ez` | Package entry and the module-level `re*` helpers |
| `pattern.ez` | `Regex` — a compiled pattern |
| `match.ez` | `Match` — one match and its position |
| `patterns.ez` | `PATTERNS`, the common pattern strings |
| `validators.ez` | `isEmail`, `isIpv4`, `isUuid`, … |
| `extract.ez` | `extractEmails`, `extractUrls`, `stripHtml`, … |
| `errors.ez` | `RegexError` |

## Flags

A string, so they combine: `"i"`, `"m"`, `"im"`.

| Flag | Effect |
| --- | --- |
| `i` | Case-insensitive |
| `m` | Multiline — `^` and `$` also match at line breaks |

The engine is ECMAScript. It has **no dotall and no named groups**, so this
package does not offer them rather than faking them. An unknown flag is an
error, not a silent no-op.

## Compiled patterns

Build a `Regex` once when the same pattern is used repeatedly.

```ez
p = Regex("[0-9]+")        # or compile("[0-9]+", "i")
```

An invalid pattern throws `RegexError` **at construction**, so the problem is
reported where it was written rather than at some later call.

| Method | Result |
| --- | --- |
| `test(text)` | Does it occur anywhere? |
| `fullMatch(text)` | Does it match the whole string? |
| `find(text, start=0)` | First `Match` at or after `start`, else `nil` |
| `findAll(text, limit=0)` | Array of `Match` |
| `findAllText(text, limit=0)` | Array of strings |
| `capture(text, n=1)` | Text of group `n` of the first match, else `nil` |
| `replace(text, repl, limit=0)` | Replace matches; `limit=0` is all |
| `replaceFirst(text, repl)` | Replace one |
| `split(text, limit=0)` | Split on the pattern |
| `count(text)` | Number of matches |

## Match objects

```ez
m = reFind("2026-08-07", "([0-9]{4})-([0-9]{2})-([0-9]{2})")
m.text          # "2026-08-07"
m.start         # 0
m.end           # 10
m.length()      # 10
m.group(0)      # whole match
m.group(2)      # "08"
m.groupCount()  # 3
```

Positions come from the engine. That matters: the previous implementation found
a match's position by searching the subject for the matched text, so `\bcat\b`
against `"concat cat"` reported index 3 — inside `"concat"` — instead of 7.

A capture group that did not participate is `nil`, not `""`. That is the only
way to tell `(a)?` that failed to match from one that matched empty.

## One-off helpers

These compile the pattern on the spot; for a loop, build a `Regex` instead.

```ez
reTest(text, pattern, flags="")
reFullMatch(text, pattern, flags="")
reFind(text, pattern, flags="")                       -> Match | nil
reFindAll(text, pattern, flags="", limit=0)           -> [Match]
reFindAllText(text, pattern, flags="", limit=0)       -> [string]
reCapture(text, pattern, n=1, flags="")               -> string | nil
reReplaceAll(text, pattern, replacement, flags="")
reReplaceFirst(text, pattern, replacement, flags="")
reSplit(text, pattern, flags="", limit=0)             -> [string]
reCount(text, pattern, flags="")
reEscape(text)
reIsValid(pattern)                                    -> bool
```

They keep the `re*` prefix deliberately: exporting `split`, `replace` or `find`
would shadow the interpreter's builtins in every file that imported this
package.

In a replacement string, `$1`–`$9` are capture groups and `$&` is the whole
match:

```ez
reReplaceAll("John Smith", "(\\w+) (\\w+)", "$2, $1")   # "Smith, John"
```

## Building patterns

```ez
reEscape("a.b")            # "a\.b" — match it literally
reAny(["cat", "dog"])      # "(cat|dog)", each alternative escaped
reWord("cat")              # "\bcat\b" — whole word only
```

## Validators

Each asks "is the **whole** string one of these?", so `isEmail("a@b.co junk")`
is false. `nil` and non-strings are false rather than an error, since these are
normally called on unvalidated input.

```
isEmail  isPhone  isPhoneIntl  isUrl  isDomain  isIpv4  isMac  isPort
isDigits  isInteger  isNumber  isAlpha  isAlphanumeric  isWhitespace
isDateIso  isTime24  isDateTimeIso
isUuid  isHexColor  isSlug  isSemver  isSsn
isCreditCardShape  isCreditCard
```

`isIpv4` bounds each octet, so `999.1.1.1` is rejected — the older
digit-counting pattern accepted it.

`isCreditCard` runs the **Luhn checksum**, which is what actually catches a
mistyped digit; `isCreditCardShape` only confirms the grouping.

## Extractors

Every occurrence, as an array of strings (empty when there is nothing to find).

```
extractEmails  extractUrls  extractPhones  extractIpv4
extractHashtags  extractMentions  extractNumbers  extractDates
extractUuids  extractHtmlTags  extractWords
extractLinkTexts  extractLinkTargets
stripHtml
```

`stripHtml` is for display and logging. It is **not** a sanitiser — it does not
decode entities and is not a parser. Do not use it on untrusted markup you are
about to render.

## Pattern strings

`PATTERNS` holds the raw strings, so you can build on them:

```ez
Regex("^" + PATTERNS["EMAIL"] + "$")
Regex(PATTERNS["URL"]).findAll(page)
```

34 entries covering primitives, contact details, network, date/time, codes and
identifiers, content and paths.

## Errors

An invalid pattern throws rather than quietly matching nothing — the worst
failure mode for a validator is one that silently says "no match":

```ez
try {
    reTest(text, userPattern)
} catch (e) {
    out "bad pattern: " + str(e)
}
```

Use `reIsValid(pattern)` to check a user-supplied pattern up front.

## Tests

```
ez regex/test_regex.ez
```
