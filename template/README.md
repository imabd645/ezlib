# template — safe templating for EZ

> **Import:** `use "template"`
> **Install:** `ez install template`

A templating engine in pure EZ. Variables, filters, conditionals and loops,
with **HTML escaping on by default**.

Pairs naturally with `web` for pages and `mailer` for HTML email.

```ez
use "template"

html = Template.render("<h1>Hello {{ name }}</h1>", { "name": userInput })
```

If `userInput` is `<script>alert(1)</script>`, the output is
`&lt;script&gt;alert(1)&lt;/script&gt;` — text on the page, not code in the
browser.

---

## Why escaping is the default

Rendering user data into HTML without escaping is how cross-site scripting
happens. Making safety the default and requiring `| raw` to opt out means that
forgetting produces visibly wrong output — escaped tags on the page — rather
than a silent vulnerability. Getting it wrong is loud instead of dangerous.

```ez
Template.render("{{ x }}", { "x": "<b>hi</b>" })         # &lt;b&gt;hi&lt;/b&gt;
Template.render("{{ x | raw }}", { "x": "<b>hi</b>" })   # <b>hi</b>
```

Only use `raw` for markup you produced or have already sanitised.

For non-HTML output — plain-text email, generated config, source code —
escaping is wrong, so turn it off explicitly:

```ez
Template.renderText("Dear {{ name }},", { "name": "O'Brien" })   # Dear O'Brien,
```

## Quick Start

```ez
use "template"

page = "<h1>{{ title }}</h1>
{% if items %}
<ul>
{% for item in items %}
  <li>{{ loop.index }}. {{ item.name | upper }}</li>
{% endfor %}
</ul>
{% else %}
<p>Nothing here yet.</p>
{% endif %}"

out Template.render(page, {
    "title": "Inventory",
    "items": [{ "name": "bolt" }, { "name": "washer" }]
})
```

## API

| Call | Result |
| --- | --- |
| `Template(source)` | Compile a template for repeated rendering. |
| `t.render(context)` | Render a compiled template. |
| `t.noEscape()` | Turn off HTML escaping. Returns the template. |
| `Template.render(source, context)` | Compile and render in one call. |
| `Template.renderText(source, context)` | Same, without HTML escaping. |
| `Template.renderFile(path, context)` | Compile and render a file. |
| `Template.compile(source)` | Same as `Template(source)`. |
| `Template.isValid(source)` | `true` when the source compiles. |
| `Template.escape(text)` | HTML-escape a string directly. |

**Compile once when a template is reused.** Parsing costs time proportional to
template size; rendering only to the data. In a request handler, compile at
startup:

```ez
PAGE = Template(readFile("views/page.html"))

@app.get("/")
task home() {
    give PAGE.render({ "user": currentUser })
}
```

## Syntax

### Variables — `{{ }}`

```
{{ name }}              a context key
{{ user.email }}        nested lookup
{{ items.0 }}           index into an array
{{ items.length }}      length of an array or string
{{ "literal" }}         a string literal
{{ 42 }}                a number literal
```

A missing key renders as empty rather than failing. Templates routinely run
against optional data, and erroring on an absent field would make them
unusable.

### Filters — `|`

Chainable, with optional arguments:

```
{{ name | upper }}
{{ name | default("anonymous") }}
{{ body | truncate(100, "…") }}
{{ tags | join(", ") }}
{{ title | trim | capitalize }}
```

| Filter | Effect |
| --- | --- |
| `upper` / `lower` | Change case |
| `capitalize` | First letter upper, rest lower |
| `title` | Capitalise Each Word |
| `trim` | Strip surrounding whitespace |
| `length` | Length of a string, array or mapping |
| `default(x)` | `x` when the value is nil or empty |
| `join(sep)` | Join an array, default separator `", "` |
| `first` / `last` | Ends of an array |
| `reverse` | Reverse a string or array |
| `replace(a, b)` | Substring replacement |
| `truncate(n, suffix)` | Cut to `n` characters, default suffix `...` |
| `round` / `abs` | Numeric |
| `urlencode` | Percent-encode for URLs |
| `json` | Serialise to JSON |
| `escape` | Force HTML escaping |
| `raw` / `safe` | **Disable** escaping for this output |

An unknown filter name throws, so a typo is reported rather than silently
rendering the unfiltered value.

### Conditionals — `{% if %}`

```
{% if user %}Welcome{% elif guest %}Hello{% else %}Sign in{% endif %}
```

Comparisons and boolean operators:

```
{% if count > 10 %}         {% if name == "admin" %}
{% if a and b %}            {% if a or b %}
{% if not disabled %}       {% if status != "done" %}
```

**Falsy values** are `nil`, `false`, `0`, `""`, and empty arrays and mappings —
so `{% if items %}` does the obvious thing for an empty list.

### Loops — `{% for %}`

```
{% for item in items %}{{ item }}{% endfor %}
```

An `{% else %}` branch renders when there is nothing to iterate:

```
{% for row in rows %}
  {{ row.name }}
{% else %}
  No results.
{% endfor %}
```

Inside a loop, `loop` describes the current iteration:

| Field | Meaning |
| --- | --- |
| `loop.index` | 1-based position |
| `loop.index0` | 0-based position |
| `loop.first` | `true` on the first item |
| `loop.last` | `true` on the last item |
| `loop.length` | Total count |

```
{% for x in xs %}{{ x }}{% if not loop.last %}, {% endif %}{% endfor %}
```

Loops iterate arrays, mapping keys, and the characters of a string. The loop
variable is scoped to the loop and does not leak out of it.

### Comments — `{# #}`

```
{# not rendered, and its contents are never evaluated #}
```

### Whitespace control — `-`

Block tags otherwise leave the newlines around them in the output. A `-` next
to a delimiter trims the whitespace on that side:

```
{% for x in xs -%}
    {{ x }}
{%- endfor %}
```

## Errors

Malformed templates throw at **compile** time with a line number, so a broken
template fails when it is loaded rather than midway through a response:

```ez
try {
    tpl = Template(source)
}
catch (e) {
    out e.message      # Template error (line 4): 'if' was never closed with 'endif'
}
```

Unknown filters throw at render time, since the filter name may come from a
value. Use `Template.isValid(source)` to check without catching.

## Deliberate limits

Templates are not a programming language here. There is no assignment, no
arithmetic, no function calls, and no way to reach outside the context you
pass in. Logic belongs in EZ where it can be tested, and a template that
cannot compute also cannot loop forever or read data you did not hand it.

Also not supported: template inheritance (`extends`/`block`), includes, and
macros. Compose in EZ instead — render a fragment and pass it in with `raw`:

```ez
body = Template.render(bodySource, data)
page = Template.render(layoutSource, { "content": body })   # {{ content | raw }}
```

## Testing

```
ez test.ez
```

91 tests covering lookups, every filter, conditionals, loops, whitespace
control, escaping and error reporting.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `Template` model |
| `lexer.ez` | Source into text/output/tag tokens; whitespace control |
| `parser.ez` | Tokens into a node tree; block matching |
| `expr.ez` | Path lookup, literals, conditions, filter chains |
| `filters.ez` | The built-in filter set and HTML escaping |
| `render.ez` | Node tree into output text; loop scoping |

## License

MIT
