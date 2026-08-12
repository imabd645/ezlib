# i18n — translation and localisation for EZ

> **Import:** `use "i18n"`
> **Install:** `ez install i18n`

Nested catalogues, interpolation, CLDR plural rules, and locale-aware number
and currency formatting.

```ez
use "i18n"

I18n.load("en", { "greeting": "Hello, {name}!" })
I18n.load("fr", { "greeting": "Bonjour, {name} !" })

I18n.locale("fr")
out t("greeting", { "name": "Ali" })      # Bonjour, Ali !
```

---

## Quick Start

```ez
use "i18n"

I18n.load("en", {
    "nav": { "home": "Home", "settings": "Settings" },
    "files": { "one": "{count} file", "other": "{count} files" }
})

I18n.locale("en")
I18n.fallback("en")

out t("nav.home", nil)          # Home
out tn("files", 1, nil)         # 1 file
out tn("files", 5, nil)         # 5 files
```

Keys are dotted paths into a nested catalogue, so translations can be
organised the way the interface is.

## Missing translations are visible

A missing key falls back to the fallback locale, and then to **the key
itself** — never to a blank. An empty string vanishes into a layout and ships
unnoticed; `nav.checkout` on a button does not.

```ez
t("nav.nonexistent", nil)       # "nav.nonexistent"
```

Unknown placeholders are left as written for the same reason:

```ez
t("greeting", {})               # "Hello, {name}!"
```

Wire up a handler to find gaps before users do:

```ez
I18n.onMissing(|key| { Log.warn("missing translation: " + key)  give nil })
```

## Plurals are not singular-vs-plural

English has two forms, so `1 file / N files` looks like the whole problem. It
is not. Russian has three categories that depend on the last two digits,
Arabic has six, Japanese has one. A library offering only singular and plural
cannot express any of them.

Catalogues use CLDR categories — `zero`, `one`, `two`, `few`, `many`,
`other`:

```ez
I18n.load("ru", {
    "files": {
        "one":  "{count} файл",       # 1, 21, 31...
        "few":  "{count} файла",      # 2-4, 22-24...
        "many": "{count} файлов"      # 5-20, 25-30...
    }
})

I18n.locale("ru")
tn("files", 2, nil)      # few
tn("files", 5, nil)      # many
tn("files", 21, nil)     # one
```

`{count}` is always available to the template. A partial table still resolves
— a missing category falls back to `other`, then to any form present, because
the wrong plural beats showing a raw key.

Rules are included for English, French, Portuguese, Russian, Ukrainian,
Belarusian, Polish, Czech, Slovak, Arabic, Welsh, Latvian, and the
single-form languages (Japanese, Chinese, Korean, Vietnamese, Thai,
Indonesian, Malay, Burmese, Khmer, Lao). Anything else behaves like English.

Note French counts **0 as singular** — `pluralCategory("fr", 0)` is `one`.
That is correct, and the kind of thing a hand-rolled `count == 1` check gets
wrong.

## Numbers and currency

Separators are not universal. Most of Europe writes `1.234,50` where English
writes `1,234.50`, which means `1,50` reads as one and a half.

```ez
I18n.locale("en")
I18n.number(1234567.891, 2)     # 1,234,567.89
I18n.currency(1234.5, "$")      # $1,234.50

I18n.locale("de")
I18n.number(1234567.891, 2)     # 1.234.567,89
I18n.currency(1234.5, "EUR")    # 1.234,50 EUR
```

Symbol placement follows the locale too — before the amount in English,
after it across most of Europe.

> **Do not format money this way.** `I18n.number` works on floats, and floats
> cannot represent tenths: `I18n.number(1.005, 2)` gives `1.00`, because
> 1.005 is really 1.00499999… Use `decimal` for anything that must balance,
> and this only for display of already-exact values.

## API

| Call | Result |
| --- | --- |
| `I18n.load(locale, table)` | Add translations; merges deeply with what is there |
| `I18n.locale(code)` | Set, or read with nil |
| `I18n.fallback(code)` | Locale used when a key is missing |
| `I18n.locales()` | Loaded locale codes |
| `I18n.t(key, values)` / `t(key, values)` | Translate |
| `I18n.plural(key, count, values)` / `tn(...)` | Translate with a count |
| `I18n.has(key)` | Whether the current locale has it |
| `I18n.onMissing(handler)` | Called with each missing key |
| `I18n.clear()` | Drop every catalogue |
| `I18n.number(value, decimals)` | Locale-aware grouping |
| `I18n.currency(value, symbol)` | Locale-aware placement |
| `pluralCategory(locale, count)` | The CLDR category, exposed for testing |

Catalogues live in a module-level store, so `load` and `locale` affect the
whole program. Load them once at startup.

## Loading from files

Pairs naturally with `yaml` or `json`:

```ez
use "yaml"
get code in ["en", "fr", "de"] {
    I18n.load(code, YAML.load("locales/" + code + ".yaml"))
}
```

Because `load` merges, a catalogue can be split across several files.

## Limits

- **Global state**, not per-request. A server handling several locales at once
  must set the locale per request, and that is not thread-safe.
- **No date or time formatting** — use `datetime`.
- **No message-format syntax** (nested selects, ordinals, gender).
- **No right-to-left handling**; that is a rendering concern.

## Testing

```
ez test.ez
```

79 tests covering lookup and fallback, deep merging, interpolation, the plural
rules for every supported family, and number and currency formatting per
locale.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `I18n` model: catalogues, lookup, interpolation, formatting |
| `plural.ez` | CLDR plural category rules and form selection |

## License

MIT
