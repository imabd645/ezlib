# progress — progress bars and spinners for EZ

> **Import:** `use "progress"`
> **Install:** `ez install progress`
> **Depends on:** `color`

```ez
use "progress"

bar = ProgressBar(len(files))
get file in files {
    process(file)
    bar.step()
}
bar.finish()
```

```
[==================>           ]  62% (31/50) ETA 12s
```

---

## Rendering and drawing are separate

`render()` builds the line; `draw()` puts it on screen in place. Keeping them
apart means the same bar can be written to a log, returned from a status
endpoint, or animated in a terminal.

```ez
bar.render()      # the string
bar.draw()        # redraw in place
bar.step()        # increment, then draw
bar.finish()      # fill, draw, end the line
```

## API

### Progress bars

| Call | Effect |
| --- | --- |
| `ProgressBar(total)` | New bar; `0` total is allowed and never divides by zero |
| `.update(n)` / `.increment()` / `.advance(n)` | Set or move the value |
| `.titled(text)` | Label before the bar |
| `.sized(cells)` | Bar width, default 30 |
| `.solid()` | `█`/`░` instead of `=`/`>` |
| `.chars(filled, head, empty)` | Custom characters |
| `.withEta()` | Show an estimate |
| `.withoutCount()` / `.withoutPercent()` | Trim the suffix |
| `.colored()` | Cyan while running, green when complete |
| `.percent()` / `.ratio()` / `.isComplete()` | State |
| `.elapsed()` / `.eta()` / `.rate()` | Timing |
| `.summary()` | The line plus elapsed time |
| `.finish()` / `.stopHere()` | End filled / end where it is |

Values outside the range are clamped, so a miscounted loop cannot produce a
bar longer than its box or a negative one.

**`eta()` returns `-1` until it has something to extrapolate from.** Guessing
a remaining time from a single sample is worse than admitting it is unknown.

### Spinners

For work with no measurable total — a spinner says "still running", which is
all it can honestly claim.

```ez
s = Spinner("connecting").dots().colored()
while working { s.tick() }
s.done("connected")
```

| Call | Effect |
| --- | --- |
| `Spinner(label)` | New spinner |
| `.tick()` | Advance one frame and redraw |
| `.dots()` | Braille frames, smoother where the font exists |
| `.withFrames(list)` | Custom frames |
| `.colored()` | Cyan frames |
| `.done(message)` / `.failed(message)` | Replace with ✓ / ✗ and end the line |

### Formatting

```ez
humanBytes(1536)        # "1.5 KB"
humanBytes(1048576)     # "1 MB"
formatSeconds(90)       # "1m30s"
formatSeconds(3700)     # "1h1m"
```

## In-place drawing needs `write`

Animation requires output with no trailing newline, which is the `write`
builtin. **On an interpreter without it, `draw()` falls back to printing whole
lines** — noisy in a terminal, but perfectly readable in a log, and it never
fails.

Each redraw pads with trailing spaces to clear whatever the previous, longer
line left behind; without that, `100%` drawn over `9%` leaves a stray digit.

## Notes

- **Not thread-safe.** Two bars drawing to the same terminal will interleave.
- **Redirected output.** In a file or CI log, every redraw becomes a separate
  line. Draw less often, or use `render()` and log at intervals.
- **Colour keeps the visible width unchanged** — widths are measured with
  `color`'s escape-aware, character-accurate helper, so a coloured bar does
  not jitter.
- Wide (CJK) label characters count as one column though they render as two.

## Testing

```
ez test.ez
```

62 tests covering clamping, zero totals, constant bar width across every
value, ETA behaviour, colour not changing visible width, spinner frame
cycling, and both output paths not throwing.

## License

MIT
