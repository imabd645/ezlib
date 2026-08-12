# text

String utilities for EZ that count **characters, not bytes**.

```ez
use "text"

slug("Crème Brûlée & Co.")        # "creme-brulee-and-co"
truncate("a long sentence", 10)   # "a long se…"
snakeCase("parseHTTPResponse")    # "parse_http_response"
countOf(3, "person")              # "3 people"
similarity("kitten", "sitting")   # 0.571…
```

## Install

```
ez install text
```

No dependencies.

## Why

EZ strings are UTF-8 and `len()` counts bytes:

```ez
len("café")        # 5
charCount("café")  # 4
len("日本語")       # 9
charCount("日本語")  # 3
```

That is the right answer for a buffer and the wrong one for anything a person
reads. `substr(name, 0, 10)` cuts a character in half; `padEnd` misaligns a
table column; reversing a string shreds it. Every function here that talks
about a length, a width or a position means characters.

## Slugs

```ez
slug("Hello, World!")             # "hello-world"
slug("Crème Brûlée")              # "creme-brulee"
slug("Größe Straße")              # "grosse-strasse"
slug("Tom & Jerry")               # "tom-and-jerry"
slug("Tom's Diner")               # "toms-diner"
slug("hello world", "_")          # "hello_world"
```

Accented letters are folded to ASCII rather than dropped, so a title in French
or German keeps its words. Both cases are in the fold table, because `lower()`
only folds ASCII — without the upper-case entries, `"CAFÉ"` would slug to
`"caf"`.

Apostrophes disappear rather than separating: `"Tom's Diner"` is `toms-diner`,
not `tom-s-diner`. Runs of anything else collapse to one separator, and the
ends are trimmed.

## Case

```ez
camelCase("user_first_name")      # "userFirstName"
pascalCase("user first name")     # "UserFirstName"
snakeCase("userFirstName")        # "user_first_name"
kebabCase("userFirstName")        # "user-first-name"
constantCase("userFirstName")     # "USER_FIRST_NAME"
titleCase("hello-world")          # "Hello World"
humanise("user_first_name")       # "User first name"
sentenceCase("hello World")       # "Hello World"
```

All of them go through `splitWords`, which handles the case that catches naive
implementations:

```ez
splitWords("parseHTTPResponse")   # ["parse", "HTTP", "Response"]
```

An acronym is one word, and the next capitalised word starts a new one — not
`["parse", "H", "T", "T", "P", "Response"]`.

`sentenceCase` only touches the first letter, so names and acronyms in the
middle of a sentence survive.

## Truncating and wrapping

```ez
truncate(text, 40)                       # "…" included in the 40
truncate(text, 40, "...")
truncateWords(text, 40)                  # cuts on a word boundary
wrap(paragraph, 72)
```

The width **includes** the suffix, so the result never exceeds it — a
truncation that overflows its column is worse than no truncation.

`wrap` breaks a word that is longer than the width rather than letting it
overflow, so a URL in a paragraph does not push the whole block out. Blank
lines are preserved.

## Layout

```ez
padStart("7", 3, "0")             # "007"
padEnd("café", 6, ".")            # "café.."   — two dots, not one
center("ab", 6, "-")              # "--ab--"
indent(block, 4)
dedent(block)
squish("  a   b  ")               # "a b"
```

`dedent` removes the common leading indentation and keeps the relative
structure. `indent` leaves blank lines alone rather than filling them with
trailing whitespace.

## Words and numbers

```ez
pluralise("city", 2)              # "cities"
pluralise("person", 2)            # "people"
pluralise("Person", 2)            # "People"  — the case is kept
countOf(1, "file")                # "1 file"
countOf(3, "file")                # "3 files"
ordinal(11)                       # "11th"   — not "11st"
article("hour")                   # "an"
listSentence(["a", "b", "c"])     # "a, b and c"
```

The irregulars are there, `-y` after a consonant becomes `-ies` while `"day"`
stays `"days"`, and 11/12/13 are handled.

## Comparing

```ez
editDistance("kitten", "sitting")           # 3
similarity("kitten", "sitting")             # 0.571…
closestMatch("instal", COMMANDS)            # "install", or nil
```

`closestMatch` is for "did you mean…". It returns `nil` when nothing is close
enough rather than confidently suggesting something wrong. Distance is counted
in characters, so `"café"` and `"cafe"` differ by one, not three.

## Redacting

```ez
mask("sk-1234567890")             # "*********7890"
maskEmail("someone@example.com")  # "s******@example.com"
initials("Ada Lovelace")          # "AL"
```

Enough left to tell two values apart, not enough to use one.

## Excerpts

```ez
excerpt(document, "search phrase", 40)
```

The text around a match, with ellipses only where something was actually cut.

## Unicode helpers

```ez
use "text/unicode.ez"

charCount(text)    chars(text)      charAt(text, i)
charSlice(text, start, count)       reverseText(text)    isAscii(text)
```

## Tests

```
ez test.ez
```

177 assertions. The non-ASCII cases are not decoration — every
character-oriented function is wrong by default until proven otherwise, and a
name with an accent in it is the input that finds it.

## License

MIT
