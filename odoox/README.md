# odoox

An Odoo JSON-RPC client for EZ.

```ez
use "odoox"

client = Odoo()
client.url = "https://mycompany.odoo.com"
client.db = "mycompany"
client.email = "admin@mycompany.com"
client.pass = "secret"

session = client.login()

rows = session.read("res.partner", ["name", "email"])
    .condition("id=87429")
    .OR("id=8129")
    .limit(10)
    .exec()
```

## Install

```
ez install odoox
```

Depends on `httpx`. Windows only, same as `httpx` — it goes through WinHTTP.

## Why `/jsonrpc` and not the web session

Odoo also has a browser-facing session API (`/web/session/authenticate`,
cookie-based). This talks to the classic `/jsonrpc` endpoint instead: `login`
on the `common` service gets a `uid`, and every later call carries
`db, uid, pass` on the `object` service's `execute_kw`. No cookie jar, no
session expiry to handle mid-script — the same approach `odoorpc` and the
official XML-RPC examples use.

## Login

```ez
client = Odoo()
client.url = "https://mycompany.odoo.com"
client.db = "mycompany"
client.email = "admin@mycompany.com"
client.pass = "secret"

session = client.login()   # throws if the credentials are wrong
```

## Reading

```ez
session.read("res.partner")                          # all fields
session.read("res.partner", ["name", "email"])        # nil fields = all fields
session.read("res.partner", nil)                      # same as above, explicit
```

`.read()` returns a `ReadQuery`. Nothing hits the network until `.exec()`.

```ez
rows = session.read("res.partner", ["name", "email"])
    .condition("id=87429")
    .OR("id=8129")
    .limit(10)
    .offset(0)
    .orderBy("name asc")
    .exec()

one = session.read("res.partner").condition("email='a@b.com'").first()
```

### Conditions

`.condition("field=value")` parses into an Odoo domain triple:
`"id=87429"` → `["id", "=", 87429]`.

| operator | means |
|---|---|
| `=` | equals |
| `!=` | not equals |
| `>`, `>=`, `<`, `<=` | comparison |

Values are auto-typed: `87429` → int, `12.5` → float, `'Bob'` or `"Bob"` →
string (quotes stripped), `true`/`false`/`nil` → themselves, anything else is
left as a string.

`.condition(...)` combines with what's there so far using **AND**.
`.OR(...)` combines with what's there so far using **OR** — so
`.condition(a).OR(b)` means `a OR b`, and `.condition(a).condition(b).OR(c)`
means `(a AND b) OR c`. This follows how Odoo domains actually evaluate
(prefix notation, implicit AND between leftover terms), not a naive AND-list
with OR bolted on.

## Writing

```ez
session.write("res.partner")
    .value("name", "Updated Name")
    .value("email", "new@b.com")
    .condition("id=87429")
    .exec()
```

Odoo's `write` takes record ids, not a domain, so `.condition(...)` here runs
a `search()` first and then `write()`s whatever matched — two RPC calls under
one chain. If you already have ids, skip the search:

```ez
session.write("res.partner").value("name", "x").ids([87429, 8129]).exec()
```

`.exec()` throws if you call it with no `.value(...)` at all, and throws if
you call it with neither `.condition(...)` nor `.ids(...)` — a write with no
target would otherwise silently hit every row.

## Inspecting a model

```ez
fields = session.inspect("res.partner")
# { "name": { "type": "char", "string": "Name", "required": true, ... }, ... }

fields = session.inspect("res.partner", ["type", "string"])   # fewer attributes
```

Wraps `fields_get`.

## Other Session methods

```ez
session.create("res.partner", { "name": "New Co" })     # -> new id
session.unlink("res.partner", [87429])                   # -> true
session.count("res.partner")                              # -> total rows
session.count("res.partner", "active=true")               # -> filtered count
```

## Errors

A bad login throws with the url/db/email it tried:

```
odoox: login failed for admin@mycompany.com on database "mycompany" -- check url, db, email, pass
```

An RPC-level error (bad model name, access rights, a Python traceback from
Odoo) throws with the service, method and Odoo's own message:

```
odoox: object.execute_kw failed: (Access Denied) You are not allowed to access 'Contact' (res.partner) records.
```

A transport failure (DNS, TLS, timeout) throws from `httpx` underneath with
the reason in words — see the `httpx` README.

## Testing without a live Odoo

`session.odoo.http` is a plain `httpx` `Http` client, so swap its transport
for a fake one the same way you would for any `httpx` consumer:

```ez
client.http = client.http.useTransport(fn(request) {
    give { "ok": true, "status": 200, "headers": "", "body": myFakeJsonRpcResponse }
})
```

See `test.ez` for a full example that never touches the network.

## License

MIT
