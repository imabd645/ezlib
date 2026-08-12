# markdown — Markdown to HTML for EZ

> **Import:** `use "markdown"`
> **Install:** `ez install markdown`

A Markdown renderer in pure EZ. Headings, lists, tables, code fences, links,
emphasis and blockquotes — with **escape-first rendering**, so markdown from an
untrusted author cannot inject HTML.

```ez
use "markdown"

html = Markdown.render(readFile("post.md"))
```

---

## Why escape-first

Markdown is usually written by someone other than the person rendering it —
a comment, a README, a CMS field. Most renderers allow raw HTML through and
then bolt on a sanitiser, which means safety depends on the filter being
complete.

This one escapes the entire document **before** any formatting runs, and
formatting only inserts tags into already-escaped text. Raw HTML surviving as
markup is not filtered out; there is no path by which it stays unescaped.

```ez
Markdown.render("<script>alert(1)</script>")
# <p>&lt;script&gt;alert(1)&lt;/script&gt;</p>
```

Link targets are checked separately: `javascript:`, `data:`, `vbscript:` and
`file:` URLs are refused, and unknown schemes are rejected. A refused link
keeps its label as plain text, so content is never lost — but no link is
emitted.

```ez
Markdown.render("[click](javascript:alert(1))")   # <p>click</p>
```

External links get `rel="nofollow noopener noreferrer" target="_blank"`.

The trade: inline HTML in your own trusted markdown will not render either.
That is deliberate.

## API

| Call | Result |
| --- | --- |
| `Markdown.render(source)` | HTML for a whole document |
| `Markdown.renderFile(path)` | Render a file |
| `Markdown.renderInline(text)` | Inline formatting only, no `<p>` wrapper |
| `Markdown.escape(text)` | HTML-escape without interpreting markdown |
| `Markdown.toText(source)` | Strip all formatting to readable plain text |
| `Markdown.excerpt(source, limit)` | Plain-text summary, cut on a word boundary |

`renderInline` is for titles and table cells where a block wrapper is wrong.
`toText` and `excerpt` are for search indexes, listing pages and email
fallbacks.

## Supported syntax

**Headings** — `#` through `######`, with optional closing hashes.

**Emphasis** — `**bold**`, `__bold__`, `*italic*`, `_italic_`, `~~strike~~`.
An unclosed marker stays literal rather than swallowing the rest of the line.

**Code** — inline `` `code` `` and fenced blocks with ``` or `~~~`. A language
tag becomes `class="language-x"`. Markdown inside a code block is never
interpreted.

````
```ez
out "not a heading: # hi"
```
````

**Lists** — `-`, `*`, `+` for bullets, `1.` for ordered, nested by indentation.

**Tables** — GitHub style, with the alignment row accepted and ignored:

```
| name | qty |
|------|-----|
| bolt | 12  |
```

**Blockquotes** — `>`, with nested block content.

**Rules** — `---`, `***`, `___`.

**Links and images** — `[text](url)`, `![alt](url)`, bare `https://` autolinks.
Parentheses inside a URL are balanced, so `[a](/x(1))` works.

**Hard breaks** — two trailing spaces become `<br>`.

## Not supported

- Raw HTML passthrough (by design — see above)
- Reference-style links (`[a][ref]`) and footnotes
- Setext headings (underlined with `===` / `---`)
- Task lists, definition lists, and HTML entities beyond escaping
- Indented (4-space) code blocks — use fences

This is a practical subset, not a CommonMark implementation. Documents relying
on the above will render as plain text rather than failing.

## Testing

```
ez test.ez
```

65 tests, including a dedicated block for the escaping and URL-scheme
behaviour.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `Markdown` model |
| `block.ez` | Line-level structure: headings, lists, tables, fences, quotes |
| `inline.ez` | Spans: emphasis, code, links; escaping and URL checking |

## License

MIT
