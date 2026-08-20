# slug

Turn any text into a URL-safe slug. Pure EZ, no dependencies.

```ez
use "slug"

slug("Café déjà vu!")    # "cafe-deja-vu"
slug("Rock & Roll")      # "rock-roll"
slug("100% Free")        # "100-free"
slug("hello world", "_") # "hello_world"
```

## What it does

1. **Lowercases** the input.
2. **Transliterates** common Latin accents to ASCII (`é → e`, `ß → ss`,
   `æ → ae`, and so on) so an accented letter becomes a letter, not a gap.
3. **Replaces** every run of non-alphanumeric characters with a single
   separator and trims the ends.

The result is safe in a URL, a filename, and an id column.

## API

| Function | Returns |
|----------|---------|
| `slug(text, sep = "-")` | slug string |

`sep` is the character (or string) used to join words; it defaults to `-`.
`nil` and `""` both return `""`.

## Notes

- Characters outside ASCII that have no transliteration mapping (CJK, emoji,
  Cyrillic) are **dropped** rather than left in the output — a raw non-ASCII
  byte is not safe in every path segment.
- The transliteration table covers the common Latin accents in both cases.
  Anything else (for example Greek or Cyrillic transliteration) is a natural
  extension but out of scope here.
