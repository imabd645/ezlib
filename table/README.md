# table — terminal tables for EZ

> **Import:** `use "table"`
> **Install:** `ez install table`
> **Depends on:** `color`

```ez
use "table"

t = Table(["Name", "Qty", "Price"])
t.align(["left", "right", "right"])
t.row(["bolt", 12, "1.50"])
t.row(["washer", 4, "0.25"])
out t.render()
```

```
┌────────┬─────┬───────┐
│ Name   │ Qty │ Price │
├────────┼─────┼───────┤
│ bolt   │  12 │  1.50 │
│ washer │   4 │  0.25 │
└────────┴─────┴───────┘
```

---

## API

| Call | Effect |
| --- | --- |
| `Table(headers)` | New table; `nil` for no header row |
| `t.row(cells)` / `t.addRows(list)` | Add rows |
| `t.fromRecords(records, fields)` | Build from a list of dictionaries |
| `t.align(list)` | Per column: `"left"`, `"right"`, `"center"` |
| `t.widths(list)` | Truncate a column at this width; `0` for unlimited |
| `t.borders(name)` | `single`, `rounded`, `double`, `ascii`, `markdown`, `none` |
| `t.spacing(n)` | Cells of padding either side, default 1 |
| `t.render()` / `t.print()` | Produce the string / print it |

All configuration methods return the table, so they chain.

> `widths()` **caps** a column, it does not widen one. Columns are always
> sized to fit their widest cell unless capped.

Numbers usually want `"right"`, so they line up on the decimal point.

## From query results

```ez
rows = db.query("SELECT name, qty FROM items")
out recordsTable(rows, ["name", "qty"])
```

A record missing a field renders blank rather than failing, so a ragged result
set still prints.

## Border styles

```ez
t.borders("ascii")      # +---+  safe in any code page or a redirected file
t.borders("rounded")    # ╭───╮
t.borders("double")     # ╔═══╗
t.borders("none")       # no box at all
t.borders("markdown")   # | a | b |  ready to paste into a README
```

Box-drawing characters look best but are not safe everywhere: a terminal in a
non-UTF-8 code page renders them as mojibake. Use `ascii` when you cannot
control where the output lands.

`markdown` emits no top or bottom rule and carries alignment in the separator
row with colons, so GitHub renders it correctly.

## Coloured cells stay aligned

This is why the package depends on `color`:

```ez
use "color"
t.row([green("ok"), "12"])
```

A coloured string is longer in bytes than it appears, so padding by `len()`
would leave every following column ragged. Widths are measured with
`visibleLength`, which ignores escape sequences **and counts characters
rather than bytes** — EZ's `len()` is byte-based, so a box-drawing character
measures 3 and an accented letter 2.

Truncation cuts on a character boundary for the same reason: slicing a
multi-byte character in half corrupts the rest of the line. A truncated cell
also has its colour stripped, because cutting mid-sequence would leave the
terminal in that colour indefinitely.

## Truncation

```ez
Table(["Path"]).widths([20]).addRows([[longPath]]).render()
```

Anything wider than the cap is cut and given an ellipsis, so one long value
cannot blow the table past the terminal width.

## Limits

- **No word wrapping.** A long cell is truncated, not wrapped onto more lines.
- **No row spanning or merged cells.**
- **No automatic terminal-width fitting** — EZ cannot query the console size,
  so set `widths()` yourself.
- Wide (CJK) characters count as one column, though they render as two, so a
  table of Chinese text will look slightly narrow.

## Testing

```
ez test.ez
```

46 tests covering sizing, alignment, every border style, truncation on
character boundaries, ragged rows, record building, and — most importantly —
that coloured and plain rows come out the same visible width.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Table` model: layout, padding, rendering |
| `borders.ez` | Border character sets |

## License

MIT
