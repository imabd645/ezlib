# html

Parse and query real web pages.

```ez
use "html"

page = parse(get("https://example.com/products").body)

get row in page.select("table.results tr") {
    cells = row.select("td")
    out cells[0].innerText() + " — " + cells[1].innerText()
}
```

## Why not `xml`

`xml` refuses a malformed document, and it is right to: in XML a missing close
tag is a bug worth surfacing. In HTML it is Tuesday.

Real pages have unclosed `<li>`, bare `<br>`, attributes with no quotes, a stray
`<` inside a script, and tags nobody closes at all. A parser that stops at the
first of those parses nothing on the open web. This one never fails — it
recovers the way a browser does, because a browser is what the page was written
against.

The recovery rules are checked against **lxml**, a browser-grade parser: 17 of
18 deliberately broken documents produce an identical tree. (The eighteenth is
a case where libxml2 and the HTML5 spec disagree; this package follows the
spec — see *Divergences*.)

## Install

```
ez install html
```

## Parsing

```ez
page = parse(source)            # a Document
```

`parse` accepts any string and never throws. Everything below hangs off the
returned `Document`.

## Selectors

Walking `children` by hand works until the page changes. `table.results tr`
survives a wrapper `div` appearing, which on a site you do not control is the
difference between a scraper that lasts a week and one that lasts a year.

```ez
page.select("div.product h2")   # every match, in document order
page.selectOne("#price")        # the first match, or nil
page.byId("main")               # shorthand
page.byTag("a")
page.byClass("item")
```

Supported:

| Selector | Matches |
| --- | --- |
| `p` | every `<p>` |
| `.item` | class |
| `#main` | id |
| `*` | any element |
| `[href]` | the attribute is present |
| `[href="/a"]` | exact value |
| `[href^="/"]` | value starts with |
| `[href$=".pdf"]` | value ends with |
| `[href*="track"]` | value contains |
| `div p` | descendant |
| `div > p` | direct child |
| `h1, h2` | either (deduplicated) |

Pseudo-classes (`:first-child`, `:not(…)`) and sibling combinators (`+`, `~`)
are **not** supported, and a selector using one raises rather than matching
approximately. An engine that quietly ignored the part it could not parse would
return the wrong elements and look like it worked — the worst outcome for a
scraper, because nothing downstream can tell.

## Elements

```ez
node.attribute("href")     # value, or nil
node.hasAttribute("data-id")
node.classes()             # ["big", "title"]
node.hasClass("active")
node.textContent()         # all descendant text, verbatim
node.innerText()           # …as it reads on the page
node.elementChildren()     # children, skipping text and comments
node.parent
node.name                  # "div"
```

`innerText` puts a boundary at every block element, which matters more than it
looks: `<p>a</p><p>b</p>` contains no whitespace at all, so plain concatenation
gives `"ab"`, and a row of `<td>ana<td>31` gives `"ana31"`. Words a page shows on
separate lines come back separated.

Attribute names are case-insensitive; values are not. A valueless attribute
(`<input required>`) has `""` as its value — ask `hasAttribute` whether it is
there.

## Whole-page helpers

```ez
page.title()
page.text()                                  # everything the page says
page.meta()                                  # name= and property= as a dict
page.links("https://site.test/docs/a.html")  # [{href, text}], resolved
page.images(base)                            # [{src, alt}], resolved
page.table("table.results")                  # rows of cell text
```

`links` and `images` take a base URL and resolve relative hrefs against it.
Without that you collect fragments you cannot fetch, which is the second thing
every scraper gets wrong.

`table` is the first. It handles the tables people actually write — no
`</td>`, no `</tr>`, no `<tbody>`:

```ez
rows = parse(source).table("table.results")
# [["Name", "Age"], ["ana", "31"], ["bo", "25"]]
```

## Text extraction

```ez
textOf(source)      # page text with script and style removed
stripTags(source)   # the same
escapeHtml(text)    # for markup you build yourself
unescapeHtml(text)  # &amp; &#39; &#x27; …
```

`textOf` drops `<script>` and `<style>` contents. Without that the "text" of a
modern page is mostly JavaScript — the single most common surprise when
extracting content.

An entity this package does not know is left exactly as it was rather than
dropped: losing text is worse than leaving it encoded, and you can still see
what it was.

## What it recovers from

```ez
parse("<ul><li>one<li>two<li>three</ul>").select("li")     # 3 siblings
parse("<div><p>a<p>b</div>").select("p")                   # 2 siblings
parse("<script>if (a < b)</script><p>x</p>").select("p")   # 1 — not eaten
parse("<input type=text name='u' required>")               # all four forms
parse("</div><p>x</p>")                                    # stray close tag
parse("<div><span>text")                                   # unclosed at EOF
parse("<h1>Title<h2>Sub")                                  # headings, siblings
```

`<script>` and `<style>` are raw text: nothing inside is markup *or* an entity.
`<title>` and `<textarea>` are RCDATA — no markup, but entities still decode, so
a title reading `Tom &amp; Jerry` comes back as `Tom & Jerry`.

## Divergences

Two, both deliberate:

- **`<h1>text<p>para`** — libxml2 makes the `<p>` a sibling of the heading; the
  HTML5 spec nests it, because a `<p>` start tag closes only an open `<p>`. This
  package follows the spec, which is what browsers do.
- **No `<tbody>` insertion, no `<html>`/`<head>`/`<body>` synthesis.** A spec
  parser inserts elements the document never had. Selectors here match what the
  page actually contains, so `table > tr` works on a table written without a
  `<tbody>`.

## Not included

- **JavaScript.** Pages that render their content client-side return their
  shell, not their data. Nothing short of a browser fixes that.
- **Serialisation back to HTML.** This parses; it does not write. Use
  `template` to generate markup.
- **XPath.** CSS selectors cover the scraping cases.

## Tests

```
ez html/test.ez
```

110 checks, plus a cross-check of 18 broken documents against lxml.

## See also

- `httpx` — fetching the pages
- `xml` — strict parsing, for documents that should be well-formed
- `template` — generating HTML
- `csv` — writing out what `table()` gives you
