# ezregex

> Comprehensive regular expression library for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**ezregex** wraps EZ's native `reMatch`, `reSearch`, and `reReplace` primitives into a high-level, batteries-included regex library. It provides an OOP `Regex` model, 30+ built-in validation patterns, extraction helpers, and a full suite of convenience functions — all in pure EZ.

---

## Installation

```
ez install regex
```

---

## Quick Start

```ez
use "regex"

# Test a pattern
out re.test("[0-9]+", "abc123")     # true

# Validate an email
out re.isEmail("user@example.com")  # true

# Extract all URLs from text
urls = re.extractUrls("Visit https://example.com and https://ez-lang.dev")
out urls  # ["https://example.com", "https://ez-lang.dev"]

# Replace
out re.replace("[0-9]+", "Price: 100 dollars", "###")
# "Price: ### dollars"
```

---

## The `Regex` Model

For full control, compile a pattern into a reusable `Regex` object:

```ez
use "regex"

r = Regex("[a-z]+@[a-z]+\\.[a-z]+")

out r.match("hello@example.com")   # true
out r.match("not-an-email")        # false
```

### Methods

| Method | Returns | Description |
|---|---|---|
| `.match(text)` | bool | Returns `true` if the entire text matches the pattern |
| `.search(text)` | dict | Finds the first match — returns `{ matched, groups, index, text }` |
| `.replace(text, replacement)` | string | Replaces the first occurrence |
| `.replaceAll(text, replacement)` | string | Replaces all occurrences |
| `.findAll(text)` | array | Returns all non-overlapping matches as `{ text, groups, index }` objects |
| `.split(text)` | array | Splits text by the pattern |
| `.getPattern()` | string | Returns the raw pattern string |

### Examples

```ez
r = Regex("\\d+")

# Search — find first match
m = r.search("Order 42 placed on day 7")
out m["matched"]   # true
out m["text"]      # "42"
out m["index"]     # 6

# Find all matches
all = r.findAll("Order 42 placed on day 7")
get match in all {
    out match["text"]   # "42", then "7"
}

# Split
parts = Regex(",\\s*").split("one, two, three")
out parts   # ["one", "two", "three"]

# Replace all
out Regex("\\s+").replaceAll("too   many   spaces", " ")
# "too many spaces"
```

---

## Quick Functions (`re` / `regex` object)

All functionality is accessible via the `re` shorthand object:

### Core Operations

```ez
re.test(pattern, text)           # → bool: true if pattern matches anywhere
re.match(pattern, text)          # → bool: full match
re.exec(pattern, text)           # → { matched, groups, text }
re.search(pattern, text)         # → array of match groups
re.replace(pattern, text, repl)  # → string: first replacement
re.replaceAll(pattern, text, r)  # → string: all replacements
re.findAll(pattern, text)        # → array of match objects
re.split(pattern, text)          # → array of strings
re.escape(text)                  # → string: special chars escaped
re.count(pattern, text)          # → int: number of matches
re.compile(pattern)              # → Regex object
```

### Pattern Helpers

```ez
# Match any of the given strings (OR)
p = re.any(["cat", "dog", "bird"])
out re.test(p, "I have a dog")    # true

# Match exact whole word
p = re.word("cat")
out re.test(p, "concatenate")     # false
out re.test(p, "my cat sat")      # true

# Case-insensitive matching
p = re.ignoreCase("hello")
out re.test(p, "HELLO world")     # true

# Anchored checks
out re.startsWith("https?://", "https://example.com")  # true
out re.endsWith("\\.json", "config.json")               # true
```

---

## Built-in Patterns

Access ready-made patterns from `re.patterns` or use them directly in `Regex()`:

```ez
use "regex"

r = Regex(re.patterns["EMAIL"])
out r.match("user@example.com")   # true

r = Regex(re.patterns["UUID"])
out r.match("550e8400-e29b-41d4-a716-446655440000")  # true
```

### Available Patterns

| Key | Matches |
|---|---|
| `DIGIT` | Single digit `0-9` |
| `NUMBER` | Integer or decimal, optional negative sign |
| `WORD` | Identifier (`[a-zA-Z_][a-zA-Z0-9_]*`) |
| `WHITESPACE` | Spaces, tabs, newlines |
| `QUOTED_STRING` | Double-quoted string with escape support |
| `SINGLE_QUOTED` | Single-quoted string with escape support |
| `EMAIL` | Standard email address |
| `PHONE_US` | US phone number (with/without formatting) |
| `PHONE_INTL` | International phone number (`+XX ...`) |
| `URL` | `http://` or `https://` URL |
| `IPV4` | IPv4 address |
| `DOMAIN` | Domain name |
| `DATE_ISO` | `YYYY-MM-DD` |
| `TIME_24H` | `HH:MM` or `HH:MM:SS` |
| `TIME_12H` | `H:MM AM/PM` |
| `DATETIME_ISO` | `YYYY-MM-DDTHH:MM:SS` |
| `HEX_COLOR` | `#RRGGBB` or `#RRGGBBAA` |
| `UUID` | Standard UUID v4 |
| `CREDIT_CARD` | 16-digit card number (various formats) |
| `SSN` | US Social Security Number |
| `HTML_TAG` | Any HTML tag |
| `HTML_COMMENT` | HTML comment block |
| `MARKDOWN_LINK` | `[text](url)` |
| `MENTION` | `@username` |
| `HASHTAG` | `#hashtag` |
| `FILENAME` | Filename with extension |
| `FILEPATH_WIN` | Windows absolute path |
| `FILEPATH_UNIX` | Unix absolute path |

---

## Validators

One-liner boolean checks for common data types:

```ez
use "regex"

out re.isEmail("hello@world.com")          # true
out re.isUrl("https://example.com/path")   # true
out re.isIpv4("192.168.1.1")               # true
out re.isDigit("00420")                    # true
out re.isAlpha("HelloWorld")               # true
out re.isAlphanumeric("abc123")            # true
out re.isWhitespace("   \t\n")             # true
out re.isDateIso("2024-01-31")             # true
out re.isUuid("550e8400-e29b-41d4-a716-446655440000")  # true
out re.isHexColor("#ff5733")               # true
out re.isCreditCard("4111 1111 1111 1111") # true
out re.isPhone("(555) 867-5309")           # true
out re.isPhoneIntl("+44 7911 123456")      # true
```

### Full Validator Reference

| Function | Validates |
|---|---|
| `re.isEmail(text)` | Email address |
| `re.isUrl(text)` | HTTP/HTTPS URL |
| `re.isIpv4(text)` | IPv4 address |
| `re.isPhone(text)` | US phone number |
| `re.isPhoneIntl(text)` | International phone number |
| `re.isDigit(text)` | Digits only |
| `re.isAlpha(text)` | Letters only |
| `re.isAlphanumeric(text)` | Letters and digits only |
| `re.isWhitespace(text)` | Whitespace only |
| `re.isDateIso(text)` | ISO date (`YYYY-MM-DD`) |
| `re.isUuid(text)` | UUID |
| `re.isHexColor(text)` | Hex color code |
| `re.isCreditCard(text)` | Credit card number |

---

## Extractors

Pull structured data out of unstructured text:

```ez
use "regex"

text = "Contact alice@example.com or bob@test.org for details."
out re.extractEmails(text)
# ["alice@example.com", "bob@test.org"]

text = "Check out https://example.com and http://ez-lang.dev/docs"
out re.extractUrls(text)
# ["https://example.com", "http://ez-lang.dev/docs"]

text = "Call +1 800-555-0100 or (212) 555-0198"
out re.extractPhones(text)
# ["+1 800-555-0100", "(212) 555-0198"]

text = "Loving #EZLang and #OpenSource today!"
out re.extractHashtags(text)
# ["#EZLang", "#OpenSource"]

text = "Thanks to @alice and @bob for the PR!"
out re.extractMentions(text)
# ["alice", "bob"]  (@ symbol stripped)

html = "<h1>Hello <b>World</b></h1>"
out re.stripHtml(html)
# "Hello World"
```

### Full Extractor Reference

| Function | Returns |
|---|---|
| `re.extractEmails(text)` | Array of email addresses |
| `re.extractUrls(text)` | Array of URLs |
| `re.extractPhones(text)` | Array of US and international phone numbers |
| `re.extractHashtags(text)` | Array of hashtags (with `#`) |
| `re.extractMentions(text)` | Array of usernames (without `@`) |
| `re.stripHtml(text)` | Text with all HTML tags removed |

---

## Complete Example — Form Validation

```ez
use "regex"

task validateForm(data) {
    errors = []

    when not re.isEmail(data["email"]) {
        push(errors, "Invalid email address")
    }

    when not re.isPhone(data["phone"]) and not re.isPhoneIntl(data["phone"]) {
        push(errors, "Invalid phone number")
    }

    when not re.isUrl(data["website"]) {
        push(errors, "Invalid website URL")
    }

    when not re.isDateIso(data["dob"]) {
        push(errors, "Date must be in YYYY-MM-DD format")
    }

    when len(errors) > 0 {
        give { "valid": false, "errors": errors }
    }
    give { "valid": true, "errors": [] }
}

result = validateForm({
    "email": "user@example.com",
    "phone": "(555) 867-5309",
    "website": "https://example.com",
    "dob": "1990-06-15"
})

out result["valid"]    # true
out result["errors"]   # []
```

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
