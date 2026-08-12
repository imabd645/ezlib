# validate — schema validation for EZ

> **Import:** `use "validate"`
> **Install:** `ez install validate`

Chainable schemas for checking untrusted input — request bodies, config files,
CSV rows — with coercion and **every error reported at once**.

```ez
use "validate"

signup = Validate.object({
    "email":    Validate.string().email(),
    "password": Validate.string().min(8).named("Password"),
    "age":      Validate.number().integer().min(13).optional()
})

result = signup.check(body)
when result["ok"] { createUser(result["value"]) }
other { give respond(400, Validate.byField(result["errors"])) }
```

---

## Design

**Checking never throws.** `check` returns every problem it found. A form that
reports one error per submission is a bad form, and an API that does the same
is worse.

**Coercion is narrow on purpose.** HTTP form fields and JSON bodies arrive as
strings, so `"12"` becomes `12` for a number field and `"true"` becomes `true`
for a bool. But `"12abc"` is rejected rather than becoming `12` — EZ's `num()`
would return `12` there, quietly accepting nonsense. Only unambiguous
conversions happen.

**The cleaned value is the output.** `result["value"]` holds coerced,
trimmed, defaulted data — use it rather than the raw input.

## API

### Building

| Call | Accepts |
| --- | --- |
| `Validate.string()` | Text. Numbers and booleans are converted to text. |
| `Validate.number()` | Numbers, and fully numeric strings. |
| `Validate.bool()` | Booleans, and `true/false/yes/no/1/0` as text. |
| `Validate.array(itemSchema)` | A list; each item is checked against `itemSchema`. |
| `Validate.object(fields)` | A mapping of field name to schema. |
| `Validate.any()` | Anything, for when only presence matters. |

### Rules

Every rule returns the schema, so they chain.

| Rule | Applies to | Meaning |
| --- | --- | --- |
| `.min(n)` | number, string, array | At least `n` — the value for numbers, the length otherwise |
| `.max(n)` | number, string, array | At most `n` |
| `.length(n)` | string, array | Exactly `n` long |
| `.notEmpty()` | string, array | Length greater than zero |
| `.integer()` | number | A whole number (`5.0` passes, `5.5` does not) |
| `.positive()` / `.negative()` | number | Strictly greater / less than zero |
| `.email()` | string | A valid email address |
| `.url()` | string | An `http`/`https` URL |
| `.slug()` | string | `lowercase-words-with-hyphens` |
| `.uuid()` | string | A UUID |
| `.oneOf(list)` | any | One of the listed values |
| `.startsWith(s)` / `.endsWith(s)` / `.contains(s)` | string | Substring checks |
| `.pattern(re, message)` | string | Matches a regex; `message` may be nil |
| `.custom(fn, message)` | any | `fn(value)` returns `true`, `false`, or a message |

### Modifiers

| Modifier | Effect |
| --- | --- |
| `.optional()` | Absent is allowed; the field is omitted from the result |
| `.nullable()` | An explicit nil is allowed and kept |
| `.withDefault(v)` | Absent becomes `v` (implies optional) |
| `.trimmed()` | Strip surrounding whitespace before checking |
| `.strict()` | *(objects)* Report fields not in the schema |
| `.named(text)` | Use `text` in messages instead of the field path |

### Checking

| Call | Result |
| --- | --- |
| `schema.check(value)` | `{ ok, value, errors }` |
| `Validate.isValid(schema, value)` | Just the boolean |
| `Validate.checkOrThrow(schema, value)` | The cleaned value, or throws listing every problem |
| `Validate.describe(errors)` | One string: `"email must be…; age must be…"` |
| `Validate.byField(errors)` | `{ "email": ["must be…"] }` — the shape form UIs want |

## The result

```ez
result = schema.check(input)

result["ok"]        # true when nothing failed
result["value"]     # coerced, trimmed, defaulted data
result["errors"]    # [{ "path": "user.email", "message": "must be a valid email address" }]
```

Paths describe where the problem is: `email`, `user.email`, `tags[2]`,
`items[0].name`. Use `.named()` when the path is not what you want a person to
read:

```ez
Validate.string().min(8).named("Password").check("abc")
# Password must be at least 8 characters
```

## Examples

**A request handler:**

```ez
use "web"
use "validate"

CREATE = Validate.object({
    "title": Validate.string().min(1).max(200).trimmed(),
    "body":  Validate.string().notEmpty(),
    "tags":  Validate.array(Validate.string().slug()).optional(),
    "draft": Validate.bool().withDefault(false)
})

@app.post("/posts")
task createPost(req) {
    result = CREATE.check(req.json())
    when result["ok"] == false {
        give { "status": 400, "errors": Validate.byField(result["errors"]) }
    }
    give savePost(result["value"])
}
```

**Config with defaults:**

```ez
CONFIG = Validate.object({
    "host":    Validate.string().withDefault("localhost"),
    "port":    Validate.number().integer().min(1).max(65535).withDefault(8080),
    "workers": Validate.number().integer().positive().withDefault(4)
}).strict()

config = Validate.checkOrThrow(CONFIG, YAML.load("config.yaml"))
```

`.strict()` turns a typo like `prot: 8080` into an error instead of silently
using the default — worth having for config files, where a mistake otherwise
goes unnoticed until something behaves oddly in production.

**A custom rule:**

```ez
Validate.number().custom(|v| v % 2 == 0, "must be even")

# Returning a string uses it as the message:
Validate.string().custom(|v| {
    when isReserved(v) { give "that name is taken" }
    give true
}, nil)
```

## Notes

- Unknown fields are **ignored** unless `.strict()` is set, so a client sending
  extra keys is not an error by default.
- Optional fields that are absent are left out of `result["value"]` entirely,
  so `has_key` stays meaningful. A `.nullable()` field that was explicitly nil
  is kept as nil.
- `.min()` means the value for numbers and the length for strings and arrays.
  The error message says which.

## Testing

```
ez test.ez
```

84 tests covering every rule, coercion, presence handling, nesting, arrays,
custom rules and error reporting.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `Validate` factory and reporting helpers |
| `schema.ez` | The `Schema` model: chaining, type handling, the walk |
| `rules.ez` | Individual rule checks and their messages |

## License

MIT
