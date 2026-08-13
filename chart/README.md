# chart

Charts as SVG, with no dependencies.

```ez
use "chart"

save("revenue.svg", line({
    "title": "Revenue",
    "labels": ["Jan", "Feb", "Mar", "Apr"],
    "series": [{ "name": "2025", "values": [12, 19, 15, 24] }]
}))
```

Numbers already arrive from `db`, `csv` and `orm`. The missing step is seeing
them. SVG is the right output: it is text, so it needs no image library; it
scales, so it survives being embedded at any size; `serve` can return it
directly, `pdf` can embed it, and every browser renders it.

## Install

```
ez install chart
```

No dependencies, no native code, no fonts to ship.

## The charts

```ez
line(options)        # trends over time
area(options)        # …with the area under it filled
bar(options)         # comparison across categories
barh(options)        # …horizontal, for long category names
pie(options)         # parts of a whole
donut(options)       # …with a hole
sparkline(values)    # small enough for a table cell
```

Every one returns SVG as a string.

## Data

Three forms, all accepted:

```ez
line({ "values": [1, 2, 3] })                       # one series
line([1, 2, 3])                                     # …the short way
line({ "series": [
    { "name": "2024", "values": [10, 12, 9] },
    { "name": "2025", "values": [14, 18, 16] }
]})
```

`nil` in a series is a **gap**, not a zero:

```ez
line({ "values": [10, nil, 30] })
```

The line breaks rather than joining across it. A missing reading plotted as
zero invents a cliff that was never in the data — and once it is in the chart,
nothing downstream can tell it was never in the data.

## Options

| Option | Default | |
| --- | --- | --- |
| `width`, `height` | 640 × 400 | |
| `title` | — | |
| `labels` | 1, 2, 3… | category names |
| `colors` | built-in palette | |
| `yLabel`, `xLabel` | — | axis titles |
| `yMin`, `yMax` | from the data | |
| `ticks` | 5 | how many axis ticks to aim for |
| `showGrid` | `true` | |
| `showLegend` | automatic | |
| `showValues` | `false` | print the numbers on the chart |
| `stacked` | `false` | bar charts only |
| `background` | white | `nil` for transparent |

An option name that is not one of these **raises**. `widht: 400` silently
ignored is a long afternoon.

## What the defaults get right

**Axes land on round numbers.** Data running 0 to 47 gets ticks at 0, 20, 40,
60 — not 0, 9.4, 18.8, 28.2, 37.6, 47. The axis is deliberately *wider* than
the data, because an axis that stops at 47 cannot have a tick at 50, and that
widening is what makes the labels readable. This is Wilkinson's rule, the same
one gnuplot and matplotlib use, and the implementation is checked against
matplotlib's `MaxNLocator` for 19 ranges.

**Bars start at zero.** A bar chart whose axis starts at 40 exaggerates every
difference; it is the commonest way a chart misleads without anyone intending
it. Line charts do not force zero, because a share price between 180 and 184
should not be a flat line at the top of an empty chart.

**Labels thin out instead of overlapping.** Two hundred categories cannot all
be printed on a 640-pixel axis, and printing them anyway produces a grey smear.

**A legend appears only when it says something.** One unnamed series does not
get one.

**Everything is escaped.** A chart is usually built from data someone else
supplied — a column of product names out of a database. An unescaped `&` is the
difference between a chart and a document the browser refuses to render.

## Examples

A stacked bar chart:

```ez
bar({
    "title": "Revenue by region",
    "labels": ["Q1", "Q2", "Q3", "Q4"],
    "series": [
        { "name": "EU", "values": [12, 15, 14, 19] },
        { "name": "US", "values": [22, 25, 27, 31] }
    ],
    "stacked": true,
    "yLabel": "£m"
})
```

Long category names read better horizontally:

```ez
barh({
    "labels": ["Wireless Keyboard", "Mechanical Keyboard", "Trackball Mouse"],
    "values": [1240, 890, 310],
    "showValues": true
})
```

Straight from a query:

```ez
use "chart"
use "db"

rows = query("SELECT month, total FROM sales ORDER BY month")
months = []
totals = []
get row in rows {
    push(months, row["month"])
    push(totals, row["total"])
}
save("sales.svg", line({ "labels": months, "values": totals, "title": "Sales" }))
```

Served directly, with no file on disk:

```ez
use "chart"
use "serve"

route("/chart.svg", task(request) {
    give { "status": 200,
           "headers": { "Content-Type": "image/svg+xml" },
           "body": line({ "values": readings() }) }
})
```

A whole dashboard in one call:

```ez
writeFile("dashboard.html", page("Today", [
    line({ "title": "Traffic", "values": hourly }),
    bar({ "title": "By source", "labels": sources, "values": counts }),
    donut({ "title": "Devices", "labels": ["Mobile", "Desktop"], "values": [64, 36] })
]))
```

Embedded in a `template`, with no separate file:

```ez
dataUri(sparkline([1, 5, 2, 8, 3]))   # data:image/svg+xml;charset=utf-8,…
```

## Output helpers

```ez
save(path, svg)      # write it out
dataUri(svg)         # for an <img src>
page(title, charts)  # a complete HTML page
colors()             # the palette, if you want to match it elsewhere
```

`dataUri` encodes the `#` in every colour, because a raw `#` ends a data URI
and the chart would silently truncate at the first fill.

## How it is tested

"Does it look right" is not an assertion, so the tests assert everything
underneath it: the axis lands where matplotlib puts it, the bars are
proportional and share a baseline, a gap stays a gap, and the markup parses as
XML.

The tick expectations come from matplotlib's `MaxNLocator`, not from running
this package and writing down what came out — a test that agrees with the code
it tests locks in whatever the code does. Every chart type is also rendered by
a real browser and checked for well-formedness and geometry.

```
ez chart/test.ez        # 114 checks
```

## Not included

- **Interactivity.** No tooltips, no zoom, no JavaScript. The output is a
  static image, which is what an emailed report, a PDF and a printed page all
  need. A dashboard that needs hovering wants a browser library.
- **Scatter and bubble charts.** Worth adding; not here yet.
- **Log scales.**
- **PNG output.** SVG is text; rasterising it needs a renderer. Any browser
  will convert one if you need pixels.

## See also

- `csv`, `db`, `orm` — where the numbers come from
- `serve` — returning a chart as a response
- `pdf` — putting one in a document
- `table` — the same data as text
