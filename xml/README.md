# xml — XML for EZ

> **Import:** `use "xml"`
> **Install:** `ez install xml`

An XML parser and builder in pure EZ. Elements, attributes, entities, CDATA
and comments, with `find`/`findAll` queries and serialisation that round-trips.

```ez
use "xml"

feed = XML.load("feed.xml")
get item in feed.findAll("item") {
    out item.child("title").value()
}
```

---

## Quick Start

```ez
use "xml"

doc = XML.parse("<library>
  <book isbn='978-1'>
    <title>The First</title>
    <author>Ali</author>
  </book>
  <book isbn='978-2'>
    <title>The Second</title>
    <author>Sara</author>
  </book>
</library>")

get book in doc.findAll("book") {
    out book.attr("isbn", "?") + " — " + book.child("title").value()
}
```

```
978-1 — The First
978-2 — The Second
```

## API

### Parsing

| Call | Result |
| --- | --- |
| `XML.parse(source)` | The root `Element`. Throws with a line number on malformed input. |
| `XML.parseOr(source, fallback)` | Same, returning `fallback` instead of throwing. |
| `XML.load(path)` | Read a file and parse it. |
| `XML.isValid(source)` | `true` when the source parses. |

### Reading an element

| Call | Result |
| --- | --- |
| `e.tag` | Tag name |
| `e.attr(name, fallback)` | Attribute, or `fallback` when absent |
| `e.hasAttr(name)` | Whether the attribute is present |
| `e.child(tag)` | First **direct** child with that tag, or nil |
| `e.childrenNamed(tag)` | All direct children with that tag |
| `e.elements()` | All direct children that are elements, in order |
| `e.find(tag)` | First **descendant** with that tag, depth-first, or nil |
| `e.findAll(tag)` | Every descendant with that tag, in document order |
| `e.value()` | Text of the subtree, trimmed — the usual way to read a leaf |
| `e.textContent()` | Text of the subtree, untrimmed |
| `e.isText()` | Whether this node is text rather than an element |

`child` searches one level; `find` searches the whole subtree. Reach for
`child` when you know the shape and `find` when you do not.

### Building

```ez
root = XML.element("order")
root.set("id", 1042)
root.add(XML.leaf("customer", "Ali"))

items = XML.element("items")
get name in ["bolt", "washer"] {
    items.add(XML.leaf("item", name))
}
root.add(items)

out XML.pretty(root)
```

```xml
<order id="1042">
  <customer>Ali</customer>
  <items>
    <item>bolt</item>
    <item>washer</item>
  </items>
</order>
```

| Call | Result |
| --- | --- |
| `XML.element(tag)` | An empty element |
| `XML.leaf(tag, text)` | `<tag>text</tag>` |
| `e.set(name, value)` | Set an attribute; returns the element |
| `e.add(child)` | Append an element; returns the parent |
| `e.addText(text)` | Append text; returns the element |

### Output

| Call | Result |
| --- | --- |
| `XML.stringify(e)` | Compact. Reflects the tree exactly and round-trips. |
| `XML.pretty(e)` | Indented for reading. |
| `XML.document(e)` | With the `<?xml ...?>` declaration, for whole files. |
| `XML.save(path, e)` | Write a complete document to a file. |

> Use `stringify`, not `pretty`, when the output will be parsed again.
> Indentation inserts text nodes that were not in the original tree, so
> `pretty` does not round-trip.

Escaping is automatic: `<`, `>` and `&` in text, plus `"` in attribute values.

### Converting to a dictionary

```ez
XML.toDict(XML.parse("<user id='7'><name>Ali</name></user>"))
# { "@id": "7", "name": "Ali" }
```

Attributes are prefixed with `@`, text sits under `#text` when an element has
both text and attributes, and repeated tags become arrays. This view is lossy —
it discards ordering and mixed content — so use it for feeding JSON, not as the
working representation.

## Details worth knowing

**Whitespace between elements is dropped.** Indentation in a source document is
formatting, not content, so `<a>\n  <b/>\n</a>` has one child, not three.
Whitespace *inside* text is preserved.

**Mixed content keeps its order.** Text is stored as a node rather than folded
onto the parent, so `<p>a <b>B</b> c</p>` survives intact:

```ez
p = XML.parse("<p>a <b>B</b> c</p>")
p.textContent()      # "a B c"
len(p.kids)          # 3 — text, element, text
```

**Entities decoded:** `&lt;` `&gt;` `&amp;` `&quot;` `&apos;`, and numeric
references in decimal (`&#65;`) and hex (`&#x41;`). An `&` that does not begin
a recognisable entity is passed through unchanged, since a bare ampersand in
otherwise usable input is common and failing on it would be unhelpful.

**CDATA is literal** — no entity decoding inside it, which is the point of it.

**Malformed input is an error, not something to repair.** Mismatched tags,
unclosed elements, unquoted attribute values and multiple roots all throw with
a line number. XML is not HTML: silently fixing a document hides the bug that
produced it.

```ez
try {
    doc = XML.parse(source)
}
catch (e) {
    out e.message    # XML parse error (line 12): end tag '</b>' does not match '<a>'
}
```

## Not supported

- Namespace resolution — `<ns:tag>` parses, but the tag name is the literal
  `"ns:tag"` and prefixes are not mapped to URIs
- DTD validation and entity definitions (a `<!DOCTYPE>` is skipped, not read)
- XPath or CSS selectors — `find`/`findAll` search by tag name only
- Attribute order is not preserved (it is not significant in XML)

## Testing

```
ez test.ez
```

66 tests covering attributes, entities, CDATA, comments, queries, mixed
content, building, round-tripping and every error case.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `XML` model — parsing, building, output, conversion |
| `node.ez` | The `Element` model, queries, entity decoding and escaping |
| `parser.ez` | Single-pass parser with an explicit tag stack |

## License

MIT
