# ezregex — Regular Expression Library for EZ

> **Version:** 1.0.0  
> **Import:** `use "ezregex"`  
> **File:** `E:\ezlib\ezregex\main.ez`

---

## Overview

`ezregex` is a comprehensive regular expression library for EZ, wrapping the built-in `reMatch`, `reSearch`, and `reReplace` primitives with a high-level OOP API. It includes:

- **`Regex` model** — Compiled pattern object
- **Global convenience functions** — Quick one-liner operations
- **Validation functions** — Common validators (email, URL, UUID, etc.)
- **Extraction functions** — Pull emails, URLs, hashtags, mentions from text
- **Pattern library** — Pre-built patterns for common use cases
- **`regex` / `re` dictionaries** — Namespace-style access to all functionality

---

## Quick Start

```ez
use "ezregex"

# Test a pattern
out reTest("[0-9]+", "hello123")   # → true

# Validate email
out isEmail("user@example.com")   # → true
out isEmail("not-an-email")        # → false

# Replace
out reReplace("Hello World", "World", "EZ")  # → "Hello EZ"

# Extract emails from text
emails = extractEmails("Contact us at info@company.com or sales@company.com")
out emails  # → ["info@company.com", "sales@company.com"]
```

---

## Model: `Regex`

An object-oriented compiled pattern.

### `Regex(pattern)`

Creates a compiled regex pattern.

```ez
use "ezregex"

r = Regex("[0-9]+")
r2 = Regex("[a-zA-Z]+@[a-zA-Z]+\\.[a-zA-Z]+")
```

---

### `r.match(text)` → `boolean`

Tests if the entire text matches the pattern. Full-string match.

```ez
use "ezregex"

r = Regex("[0-9]+")
out r.match("123")      # → true
out r.match("12abc")    # → false (not full match)
```

---

### `r.search(text)` → `dictionary`

Searches for the first occurrence of the pattern anywhere in the text.

**Returns:** Dictionary with:
- `matched` — `boolean`
- `groups` — Array of capture group strings
- `index` — `0` if found, `-1` if not
- `text` — Matched text string, or `nil`

```ez
use "ezregex"

r = Regex("[0-9]+")
result = r.search("price: 42 dollars")

out result["matched"]   # → true
out result["text"]      # → "42"
out result["groups"]    # → ["42"]

noResult = r.search("no numbers here")
out noResult["matched"]  # → false
out noResult["text"]     # → nil
```

---

### `r.replace(text, replacement)` → `string`

Replaces the **first** occurrence of the pattern.

```ez
use "ezregex"

r = Regex("[0-9]+")
out r.replace("I have 3 cats and 5 dogs", "X")
# → "I have X cats and 5 dogs"
```

---

### `r.replaceAll(text, replacement)` → `string`

Replaces **all** occurrences of the pattern (up to 1000 replacements).

```ez
use "ezregex"

r = Regex("[0-9]+")
out r.replaceAll("I have 3 cats and 5 dogs", "N")
# → "I have N cats and N dogs"

r2 = Regex("\\s+")
out r2.replaceAll("too  many   spaces", " ")
# → "too many spaces"
```

---

### `r.findAll(text)` → `array`

Finds all non-overlapping matches in text.

**Returns:** Array of match objects, each with:
- `text` — Matched string
- `groups` — Capture groups array
- `index` — Position in original string

```ez
use "ezregex"

r = Regex("[0-9]+")
matches = r.findAll("1 cat, 22 dogs, 333 birds")

out len(matches)          # → 3
out matches[0]["text"]    # → "1"
out matches[1]["text"]    # → "22"
out matches[2]["text"]    # → "333"
out matches[0]["index"]   # → 0
```

---

### `r.split(text)` → `array`

Splits text by the pattern (up to 1000 parts).

```ez
use "ezregex"

r = Regex(",\\s*")
parts = r.split("apple, banana, cherry")
out parts   # → ["apple", "banana", "cherry"]

r2 = Regex("\\s+")
words = r2.split("The quick  brown   fox")
out words   # → ["The", "quick", "brown", "fox"]
```

---

### `r.getPattern()` → `string`

Returns the pattern string.

```ez
use "ezregex"

r = Regex("[a-z]+")
out r.getPattern()  # → "[a-z]+"
```

---

## Global Convenience Functions

### `reTest(pattern, text)` → `boolean`
Quick test: does `text` match `pattern`?

```ez
use "ezregex"

out reTest("[0-9]+", "42")       # → true
out reTest("[0-9]+", "abc")      # → false
out reTest("^[A-Z]", "Hello")    # → true
out reTest("^[A-Z]", "hello")    # → false
```

---

### `reExec(pattern, text)` → `dictionary`
Execute search and return structured result.

```ez
use "ezregex"

r = reExec("\\d+", "room 404")
out r["matched"]   # → true
out r["text"]      # → "404"
```

---

### `reSplit(pattern, text)` → `array`
Global split function.

```ez
use "ezregex"

parts = reSplit("[,;]\\s*", "a, b; c, d")
out parts  # → ["a", "b", "c", "d"]
```

---

### `reEscape(text)` → `string`
Escapes special regex characters in a literal string.

Special chars escaped: `\ ^ $ . | ? * + ( ) [ ] { }`

```ez
use "ezregex"

out reEscape("hello.world")   # → "hello\\.world"
out reEscape("(a+b)")         # → "\\(a\\+b\\)"
```

---

### `reAny(strings)` → `string`
Creates an alternation pattern matching any of the given strings.

```ez
use "ezregex"

pattern = reAny(["cat", "dog", "bird"])
out pattern   # → "(cat|dog|bird)"

r = Regex(pattern)
out r.match("cat")    # → true
out r.match("fish")   # → false
```

---

### `reWord(text)` → `string`
Wraps text in word-boundary anchors `\b...\b`.

```ez
use "ezregex"

pattern = reWord("cat")
r = Regex(pattern)
out r.search("I have a cat here")["matched"]   # → true
out r.search("concatenation")["matched"]        # → false (no word boundary)
```

---

### `reIgnoreCase(pattern)` → `string`
Converts a pattern to case-insensitive by expanding each letter into `[aA]` character classes.

```ez
use "ezregex"

p = reIgnoreCase("hello")
out p  # → "[hH][eE][lL][lL][oO]"

r = Regex(p)
out r.match("Hello")   # → true
out r.match("HELLO")   # → true
out r.match("hello")   # → true
```

---

### `reStartsWith(pattern, text)` → `boolean`
Tests if text starts with pattern.

```ez
use "ezregex"

out reStartsWith("[A-Z]", "Hello")  # → true
out reStartsWith("[A-Z]", "hello")  # → false
```

---

### `reEndsWith(pattern, text)` → `boolean`
Tests if text ends with pattern.

```ez
use "ezregex"

out reEndsWith("\\.ez$", "script.ez")  # → true
out reEndsWith("\\.py$", "script.ez")  # → false
```

---

### `reCount(pattern, text)` → `number`
Counts occurrences of pattern in text.

```ez
use "ezregex"

out reCount("[0-9]+", "1 cat, 22 dogs, 333 birds")  # → 3
out reCount("\\.", "a.b.c.d")                        # → 3
```

---

## Validation Functions

All return `boolean`. They use `reMatch` with full-string anchors.

| Function | Pattern | Example |
|---|---|---|
| `isEmail(text)` | RFC-ish email | `"user@example.com"` |
| `isPhone(text)` | US phone | `"(555) 123-4567"` |
| `isPhoneIntl(text)` | International phone | `"+1 555 1234567"` |
| `isUrl(text)` | HTTP/HTTPS URL | `"https://example.com"` |
| `isIpv4(text)` | IPv4 address | `"192.168.1.1"` |
| `isDigit(text)` | Digits only | `"123"` |
| `isAlpha(text)` | Letters only | `"hello"` |
| `isAlphanumeric(text)` | Letters + digits | `"abc123"` |
| `isWhitespace(text)` | Whitespace only | `"   "` |
| `isDateIso(text)` | ISO date | `"2025-06-11"` |
| `isUuid(text)` | UUID v4 | `"550e8400-..."`|
| `isHexColor(text)` | Hex color | `"#ff5733"` |
| `isCreditCard(text)` | Credit card | `"4111 1111 1111 1111"` |

```ez
use "ezregex"

out isEmail("user@example.com")           # → true
out isEmail("not.valid")                   # → false
out isUrl("https://google.com/search?q=ez") # → true
out isIpv4("192.168.1.256")               # → false (invalid octet)
out isDigit("0042")                        # → true
out isUuid("550e8400-e29b-41d4-a716-446655440000")  # → true
out isHexColor("#ff5733")                  # → true
out isHexColor("#zzzzzz")                  # → false
```

---

## Extraction Functions

### `extractEmails(text)` → `array`
Extracts all email addresses from text.

```ez
use "ezregex"

text = "Contact info@company.com or support@example.org for help."
emails = extractEmails(text)
out emails  # → ["info@company.com", "support@example.org"]
```

---

### `extractUrls(text)` → `array`
Extracts all HTTP/HTTPS URLs.

```ez
use "ezregex"

text = "Visit https://example.com or http://docs.ez-lang.org for more info."
urls = extractUrls(text)
out urls   # → ["https://example.com", "http://docs.ez-lang.org"]
```

---

### `extractPhones(text)` → `array`
Extracts US and international phone numbers.

```ez
use "ezregex"

text = "Call us: (555) 123-4567 or +44 7911 123456"
phones = extractPhones(text)
out phones  # → ["(555) 123-4567", "+44 7911 123456"]
```

---

### `extractHashtags(text)` → `array`
Extracts hashtags (including the `#`).

```ez
use "ezregex"

text = "Loving #EZLanguage and #coding today! #tech"
tags = extractHashtags(text)
out tags   # → ["#EZLanguage", "#coding", "#tech"]
```

---

### `extractMentions(text)` → `array`
Extracts mentions (strips the `@`).

```ez
use "ezregex"

text = "Thanks @alice and @bob_smith for the help!"
mentions = extractMentions(text)
out mentions   # → ["alice", "bob_smith"]
```

---

### `stripHtml(text)` → `string`
Removes all HTML tags from text.

```ez
use "ezregex"

html = "<h1>Hello</h1><p>This is <strong>EZ</strong>.</p>"
plain = stripHtml(html)
out plain   # → "HelloThis is EZ."
```

---

## Built-in Pattern Library

Access via `patterns["KEY"]` or `regex["patterns"]["KEY"]`.

```ez
use "ezregex"

out patterns["EMAIL"]        # → "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}"
out patterns["UUID"]         # → "[0-9a-fA-F]{8}-..."
out patterns["DATE_ISO"]     # → "[0-9]{4}-[0-9]{2}-[0-9]{2}"
out patterns["HEX_COLOR"]    # → "#[0-9a-fA-F]{6}([0-9a-fA-F]{2})?"
out patterns["IPV4"]         # → "([0-9]{1,3}\.){3}[0-9]{1,3}"
out patterns["HTML_TAG"]     # → "<[^>]+>"
out patterns["URL"]          # → "https?://..."
out patterns["FILENAME"]     # → file name with extension
```

---

## `regex` / `re` Dictionary

The `regex` object and its alias `re` expose the entire API in a namespaced style:

```ez
use "ezregex"

# Pattern compilation
r = regex.compile("[0-9]+")
r = re.create("[a-z]+")

# Quick operations
out regex.test("[0-9]+", "42")           # → true
out regex.match("[0-9]+", "42")          # → true
out regex.replace("[0-9]+", "abc123", "N")  # → "abcN"
out regex.split(",", "a,b,c")           # → ["a", "b", "c"]
out regex.escape("(a+b)")               # → "\\(a\\+b\\)"

# Advanced
matches = regex.findAll("[0-9]+", "1 and 22 and 333")
out len(matches)  # → 3

# Validators (via re namespace)
out re.isEmail("x@y.com")   # → true
out re.isUrl("https://x.com")  # → true

# Extractors
out re.extractEmails("hi@a.com and bye@b.com")

# Patterns
out re.patterns["DATE_ISO"]
```

---

## Edge Cases

### findAll Loop Safety
`findAll()` is capped at 1000 matches by default to prevent infinite loops with zero-length match patterns.

### replaceAll Loop Safety
`replaceAll()` is capped at 1000 replacements. If the pattern matches the empty string, this could loop infinitely without the cap.

### reEscape Completeness
`reEscape()` uses `reReplace()` to escape each special character. The pipe `|` character is **not included** in the special chars list and may need manual escaping.

### `reSearch` Return Format
EZ's built-in `reSearch(text, pattern)` returns an array of strings (all matches/groups). `reExec()` and `Regex.search()` normalize this into the standard `{matched, groups, text, index}` structure.

### Full-String vs Partial Match
- `reMatch(text, pattern)` — **full string** match (anchored implicitly)
- `reSearch(text, pattern)` — **partial** match (finds first occurrence anywhere)

Validation functions use `reMatch` with `^pattern$` anchors to enforce exact full-string matching.

---

## Full Example: Form Validator

```ez
use "ezregex"

task validateForm(name, email, phone, website) {
    errors = []
    
    when len(name) < 2 {
        push(errors, "Name must be at least 2 characters")
    }
    
    when not isEmail(email) {
        push(errors, "Invalid email: " + email)
    }
    
    when not isPhone(phone) and not isPhoneIntl(phone) {
        push(errors, "Invalid phone number: " + phone)
    }
    
    when len(website) > 0 and not isUrl(website) {
        push(errors, "Invalid website URL: " + website)
    }
    
    when len(errors) == 0 {
        out "✓ Form valid!"
        give true
    }
    
    out "✗ Validation errors:"
    get e in errors { out "  - " + e }
    give false
}

validateForm("Alice", "alice@example.com", "(555) 123-4567", "https://alice.dev")
validateForm("B", "not-an-email", "12345", "bad-url")
```

---

## Full Example: Log Parser

```ez
use "ezregex"

logLines = [
    "2025-06-11 08:30:00 [INFO] Server started on port 8080",
    "2025-06-11 08:30:05 [WARN] Memory at 78%",
    "2025-06-11 08:30:10 [ERROR] Connection refused to db.prod:5432",
    "2025-06-11 08:31:00 [INFO] User alice@example.com logged in"
]

datePattern = Regex("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}")
levelPattern = Regex("\\[(INFO|WARN|ERROR)\\]")
emailPattern = Regex("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}")

errors = []
get line in logLines {
    level = levelPattern.search(line)
    when level["matched"] and level["text"] == "[ERROR]" {
        push(errors, line)
    }
    
    emailFound = emailPattern.search(line)
    when emailFound["matched"] {
        out "Email activity: " + emailFound["text"]
    }
}

out "\nErrors found: " + str(len(errors))
get e in errors { out "  " + e }
```

---

*Documentation generated from `E:\ezlib\ezregex\main.ez` — EZ Regex Library v1.0.0*
