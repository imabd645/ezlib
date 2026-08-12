# color — terminal colour and styling for EZ

> **Import:** `use "color"`
> **Install:** `ez install color`

```ez
use "color"

out red("error: ") + "something went wrong"
out Color().green().bold().text("done")
```

---

## Quick Start

One call for the common case:

```ez
red("x")  green("x")  yellow("x")  blue("x")
magenta("x")  cyan("x")  white("x")  gray("x")
bold("x")  dim("x")  italic("x")  underline("x")
```

Chain when you need more than one attribute:

```ez
Color().red().bold().underline().text("important")
Color().white().onRed().text(" ERROR ")
```

Semantic helpers keep a program's output consistent without every call site
picking a colour:

```ez
success("saved")     # green, bold
failure("rejected")  # red, bold
warning("careful")   # yellow
info("note")         # cyan
muted("optional")    # gray
```

## Turning it off

```ez
Color.disable()
```

Every helper then returns plain text, so calling code needs no conditionals.
Do this when output is redirected to a file, or behind a `--no-color` flag —
escape codes in a log file are noise.

## Colours

| Method | Effect |
| --- | --- |
| `.red()` `.green()` `.yellow()` `.blue()` `.magenta()` `.cyan()` `.white()` `.black()` `.gray()` | Foreground |
| `.onRed()` `.onGreen()` `.onYellow()` `.onBlue()` `.onWhite()` `.onBlack()` | Background |
| `.foreground(name)` / `.background(name)` | By name, including `brightRed` etc. |
| `.fg256(n)` / `.bg256(n)` | 256-colour palette |
| `.rgb(r, g, b)` / `.bgRgb(r, g, b)` | 24-bit truecolor |
| `.hex("#ff8800")` / `.bgHex(...)` | Truecolor from hex |
| `.bold()` `.dim()` `.italic()` `.underline()` `.inverse()` `.strike()` | Styles |
| `.text(value)` | Wrap and close |

Not every terminal renders truecolor; the 256-colour palette is the safer
choice when you cannot control the environment.

An unknown colour or style name **throws**, so a typo is reported rather than
silently producing unstyled text.

## Measuring coloured text

This is the part other libraries need. A coloured string is longer than it
looks, so aligning by `len()` leaves columns ragged:

```ez
s = red("abc")
len(s)               # 12 — bytes, including escapes
visibleLength(s)     # 3  — what actually appears
stripAnsi(s)         # "abc"
```

**`visibleLength` counts characters, not bytes.** EZ's `len()` is byte-based,
so a box-drawing character measures 3 and an accented letter 2. Padding a
column by `len()` would make every line a different width on screen.

`visibleSubstr(text, from, count)` slices by character for the same reason —
cutting a multi-byte character in half renders as a replacement glyph and can
corrupt the rest of the line.

```ez
hasAnsi(text)                     # does it contain escapes?
visibleSubstr("héllo", 0, 3)      # "hél" — never a half character
```

`table` and `progress` both build on these.

## Hex conversion

```ez
hexToRgb("#ff8800")     # [255, 136, 0]
hexToRgb("#f80")        # [255, 136, 0] — short form expands
rgbToHex(255, 136, 0)   # "#ff8800"
```

Malformed hex throws rather than producing a wrong colour.

## Notes

- **Colour is always closed.** Every wrapped string ends with a reset —
  an unclosed sequence colours everything printed afterwards, including the
  shell prompt after the program exits.
- **No terminal detection.** There is no reliable way to ask EZ whether stdout
  is a TTY, so colour is on by default and you turn it off explicitly.
- Windows Terminal, PowerShell 7 and modern consoles all render these; the
  legacy `cmd.exe` console may not.

## Testing

```
ez test.ez
```

51 tests covering code generation, chaining, stripping (including multi-
parameter sequences), character-accurate measurement, enable/disable and hex
conversion.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Color` model, one-call helpers, hex conversion |
| `codes.ez` | ANSI tables, stripping, and character-accurate measurement |

## License

MIT
