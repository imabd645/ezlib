# web — Web Framework for EZ

> **Version:** 2.0.2  
> **Import:** `use "web"`  
> **File:** `C:\ezlib\web\main.ez`  
> **Requires:** `kernel32.dll`, `ws2_32.dll`, `msvcrt.dll`, and the bundled `dlls/http_accel.dll` + `dlls/web_accel.dll`

---

## Overview

`web` is a full-featured, Flask-inspired web framework for EZ built entirely on raw Windows Sockets (Winsock2) via FFI. It provides:

- **Route registration** for GET, POST, PUT, DELETE
- **Dynamic URL parameters** (`<param>` syntax)
- **Query string** and **form body** parsing
- **Cookie and session management**
- **Static file serving**
- **Jinja2-like template engine** with `{{ var }}`, `{% if %}`, `{% for %}`, `{% extends %}`, `{% block %}`, `{% include %}`
- **Middleware** support
- **Custom 404 / 500 handlers**

---

## Quick Start

```ez
use "web"

app = new WebApp()

@app.get("/")
task home(req) {
    give "<h1>Hello from EZ Web!</h1>"
}

@app.get("/api/hello")
task hello(req) {
    give {"message": "Hello, World!", "status": "ok"}
}

app.run(8080)
# → Server starts at http://localhost:8080
```

The lambda form works identically if you prefer it:

```ez
app.get("/", |req| {
    give "<h1>Hello from EZ Web!</h1>"
})
```

### Install location

The framework loads two native accelerators (`http_accel.dll`, `web_accel.dll`) from
`<ezlib root>/web/dlls`. The root is `C:\ezlib` unless the `EZLIB_PATH` environment
variable says otherwise — the same variable the interpreter uses to find the standard
library, so a relocated ezlib needs no extra configuration:

```
set EZLIB_PATH=D:\tools\ezlib
```

If the DLLs cannot be loaded, `use "web"` throws immediately and names the directory it
looked in.

---

## Model: `WebApp`

### `WebApp()`
Creates a new web application instance.

```ez
use "web"

app = WebApp()
```

---

## Route Registration

Every registrar takes two forms. Pass the handler directly, or omit it and use the
call as a **decorator** — Flask style:

```ez
# decorator form
@app.get("/")
task home(req) {
    give { "status": 200, "body": "Hello" }
}

@app.post("/users")
task createUser(req) {
    give json_response({ "created": true }, 201)
}

# direct form — identical result
app.get("/", |req| {
    give { "status": 200, "body": "Hello" }
})
```

Both forms return the handler, so a decorated task stays callable by its own name:

```ez
@app.get("/health")
task health(req) {
    give { "status": 200, "body": "ok" }
}

out health({})     # still a normal task
```

That matters because a decorator's return value **rebinds** the decorated name. Any
decorator of your own must `give` the function back, or the task becomes `nil`:

```ez
task timed(fn) {
    give |req| {
        t = clock()
        r = fn(req)
        out "took " + str(clock() - t) + "ms"
        give r
    }
}

@app.get("/slow")
@timed
task slow(req) { ... }
```

Decorators apply bottom-up: `@timed` wraps `slow`, then `@app.get("/slow")` registers
the wrapped function.

`use`, `on_404` and `on_500` work as decorators too — they take the handler as their
only argument, so no call parentheses are needed:

```ez
@app.use
task tagger(req, res) {
    res["headers"]["X-Served-By"] = "web"
    give nil                      # nil = continue; a dictionary = short-circuit
}

@app.on_404
task missing(req) {
    give { "status": 404, "body": "Nothing here" }
}
```

### `app.get(path, handler)`
Registers a GET route.

### `app.post(path, handler)`
Registers a POST route.

### `app.put(path, handler)`
Registers a PUT route.

### `app.patch(path, handler)`
### `app.head(path, handler)`
### `app.options(path, handler)`
Same shape as the others. A request whose path matches a route registered under a
*different* method gets `405 Method Not Allowed` with an `Allow:` header (not a
misleading `404`).

### `app.on_error(code, handler)`
Registers a handler for a specific status code. `on_404` and `on_500` are the same
mechanism under fixed codes; `on_error` covers everything else the framework can
produce (405, 401, 403, …).

```ez
@app.on_error(405)
task badMethod(req) {
    give { "status": 405, "body": "Wrong verb for this route" }
}
```

### `app.after(handler)`
after_request. Runs once the response is decided, on **every** response — matched
routes, 404, 405 and 500 alike. Middleware only runs *before* the handler, so this is
where "add a header to everything" or "log what we actually answered" belongs.

```ez
@app.after
task stamp(req, res) {
    res["headers"]["X-Response-Time"] = str(clock() - req["startedAt"])
    give nil            # or return a replacement {status, headers, body}
}
```

### `app.run(port, workers = 1)`
Binds `port` and serves until stopped. `workers` > 1 runs that many worker processes so
handlers execute in parallel — see [Worker Processes](#worker-processes) for what that
changes about shared state.

```ez
app.run(8080)                     # single process
app.run(port=8080, workers=4)     # 4 worker processes
```

### `app.stop()`
Stops the listener and waits for in-flight handlers to finish. Useful for tests and
graceful shutdown.

### `app.delete(path, handler)`
Registers a DELETE route.

**Handler signature:** `|req| { give response }`

**Response types:**
- `string` → Sent as `text/html`
- `dictionary` / `array` → Auto-serialized to JSON (`application/json`)
- `dictionary` with `status`, `body`, `headers` keys → Full control response

```ez
use "web"

app = WebApp()

# HTML response
app.get("/", |req| {
    give "<h1>Welcome!</h1>"
})

# JSON response
app.get("/api/status", |req| {
    give {"status": "healthy", "version": "1.0"}
})

# Custom status code
app.get("/api/old", |req| {
    give {"status": 301, "headers": {"Location": "/api/new"}, "body": ""}
})

app.run(8080)
```

---

## Testing

### `app.test_request(method, path, opts = nil)` → `{status, headers, body, request}`

Runs a request through the app without binding a socket.

```ez
app.get("/users/<int:id>", |req| { give { "status": 200, "body": "user " + str(req["params"]["id"]) } })

r = app.test_request("GET", "/users/42")
r["status"]     # 200
r["body"]       # "user 42"
```

`opts` accepts `body`, `headers`, `form`, `query`, `cookies` and `session`. Header names
are lower-cased for you, matching what a handler sees from the real server.

This shares the matcher, middleware list, after hooks and error handlers with the live
pipeline, so routing, parameters, short-circuits and after-hooks behave identically. It
does **not** serialise to bytes, so gzip, chunked framing and keep-alive are not
exercised.

---

## Logging and Configuration

### `app.useAccessLog(sink = nil)`
One line per request — method, path, status, duration, client address. Defaults to
stdout; pass a function to route lines elsewhere.

```ez
app.useAccessLog()
# GET /users/42 200 3ms 127.0.0.1
```

### `app.config` / `config_get` / `config_set` / `config_from_env`
`app.config` is a plain dictionary.

```ez
app.config_set("mode", "production")
app.config_get("mode")                       # "production"
app.config_get("missing", "fallback")        # "fallback"

# EZWEB_PORT -> config["port"], EZWEB_SECRET -> config["secret"]
app.config_from_env("EZWEB_", ["PORT", "SECRET"])
```

`config_from_env` takes explicit names rather than enumerating the environment, because
there is no portable way to list it through the FFI.

---

## Dynamic URL Parameters

Use `<paramName>` in the route pattern to capture dynamic segments.

```ez
use "web"

app = WebApp()

app.get("/users/<id>", |req| {
    userId = req["params"]["id"]
    give {"userId": userId}
})

app.get("/products/<category>/<id>", |req| {
    cat = req["params"]["category"]
    pid = req["params"]["id"]
    give {"category": cat, "productId": pid}
})

app.run(8080)
```

### Converters

A prefix narrows what a segment will match, and converts it before the handler sees it.
A segment that does not fit the converter simply does not match that route, so it falls
through to a sibling route or to `404` — the handler never receives a value it did not
ask for.

| Pattern | Matches | `req["params"]` value |
|---|---|---|
| `<id>` | one segment | string |
| `<int:id>` | digits, optional leading `-` | integer |
| `<float:amount>` | decimal number | float |
| `<path:rest>` | the remaining segments, slashes included | string |

```ez
@app.get("/users/<int:id>")
task byId(req) { give "numeric id: " + str(req["params"]["id"]) }

@app.get("/users/<name>")
task byName(req) { give "name: " + req["params"]["name"] }

@app.get("/files/<path:rest>")
task files(req) { give req["params"]["rest"] }     # /files/a/b/c.txt -> "a/b/c.txt"
```

`/users/42` reaches `byId` with an integer; `/users/alice` reaches `byName`. A typed
parameter outranks an untyped one, and a literal segment outranks both, so the outcome
does not depend on registration or hash order. `<path:…>` only makes sense as the final
segment.

### `app.url_for(pattern, params = nil, method = "GET")` → `string`

Builds a URL from a route pattern, and **throws if that route is not registered** —
which is the point. Hand-built links (`"/users/" + str(id)`) go stale silently when a
route is renamed; `url_for` fails loudly instead.

```ez
app.url_for("/users/<int:id>", {"id": 42})        # "/users/42"
app.url_for("/search", {"q": "a b"})              # "/search?q=a+b"
```

Parameters the pattern does not consume become the query string. Path segments are
percent-encoded (`%20`), not form-encoded (`+`), since `+` is a literal plus in a path.

---

## The Request Object

Every handler receives a `req` dictionary:

| Key | Type | Description |
|---|---|---|
| `method` | `string` | `"GET"`, `"POST"`, etc. |
| `path` | `string` | URL path (without query string) |
| `fullPath` | `string` | Full URL including query string |
| `query` | `dictionary` | Parsed query parameters |
| `form` | `dictionary` | Parsed form body (for `application/x-www-form-urlencoded`) |
| `body` | `string` | Raw request body |
| `headers` | `dictionary` | HTTP headers (lowercase keys) |
| `params` | `dictionary` | Dynamic URL parameters |
| `cookies` | `dictionary` | Parsed cookies |
| `session` | `dictionary` | Session data for this user |

```ez
use "web"

app = WebApp()

app.post("/login", |req| {
    username = req["form"]["username"]
    password = req["form"]["password"]
    
    when username == "admin" and password == "secret" {
        req["session"]["user"] = username
        req["session"]["loggedIn"] = true
        give redirect("/dashboard")
    }
    
    give {"status": 401, "body": "Invalid credentials", "headers": {}}
})

app.run(8080)
```

---

## Query Parameters

```ez
use "web"

app = WebApp()

# GET /search?q=hello&page=2
app.get("/search", |req| {
    query = req["query"]["q"]
    page = req["query"]["page"]
    
    when not query { query = "" }
    when not page { page = "1" }
    
    give {
        "query": query,
        "page": num(page),
        "results": []  # would be filled from DB
    }
})

app.run(8080)
```

---

## JSON API

```ez
use "web"

app = WebApp()

users = [
    {"id": 1, "name": "Alice"},
    {"id": 2, "name": "Bob"}
]

app.get("/api/users", |req| {
    give users
})

app.post("/api/users", |req| {
    # Parse JSON body
    data = parse_json(req["body"])
    newUser = {"id": len(users) + 1, "name": data["name"]}
    push(users, newUser)
    give {"status": 201, "body": newUser, "headers": {}}
})

app.get("/api/users/<id>", |req| {
    targetId = num(req["params"]["id"])
    get u in users {
        when u["id"] == targetId { give u }
    }
    give {"status": 404, "body": "User not found", "headers": {}}
})

app.run(8080)
```

---

## Sessions

Sessions are managed automatically via a `ez_session` cookie. Session data is stored server-side in memory.

```ez
use "web"

app = WebApp()

app.get("/login", |req| {
    give "<form method='post' action='/login'>
        <input name='user'><button>Login</button>
    </form>"
})

app.post("/login", |req| {
    user = req["form"]["user"]
    req["session"]["user"] = user
    req["session"]["loggedIn"] = true
    give redirect("/dashboard")
})

app.get("/dashboard", |req| {
    when not req["session"]["loggedIn"] {
        give redirect("/login")
    }
    give "<h1>Welcome, " + req["session"]["user"] + "!</h1>"
})

app.get("/logout", |req| {
    req["session"] = {}
    give redirect("/login")
})

app.run(8080)
```

---

## Cookies

```ez
use "web"

app = WebApp()

app.get("/", |req| {
    theme = req["cookies"]["theme"]
    when not theme { theme = "light" }
    give "<p>Current theme: " + theme + "</p>"
})

app.get("/set-theme/<t>", |req| {
    t = req["params"]["t"]
    give {
        "status": 200,
        "body": "Theme set!",
        "headers": {"Set-Cookie": "theme=" + t + "; Path=/; Max-Age=86400"}
    }
})

app.run(8080)
```

---

## Template Engine

### `render_template(path, ctx)` → `string`

Renders a template file with a context dictionary. Returns the rendered HTML.

```ez
use "web"

app = WebApp()

app.get("/", |req| {
    ctx = {
        "title": "My EZ App",
        "user": "Alice",
        "items": ["Apple", "Banana", "Cherry"],
        "loggedIn": true
    }
    give render_template("templates/index.html", ctx)
})

app.run(8080)
```

**Template syntax:**

| Tag | Description |
|---|---|
| `{{ var }}` | Output a variable |
| `{{ obj.field }}` | Access nested field (dot notation) |
| `{% if var %}...{% endif %}` | Conditional block |
| `{% if var %}...{% else %}...{% endif %}` | If-else |
| `{% for item in items %}...{% endfor %}` | Loop over array/dictionary |
| `{% extends "base.html" %}` | Template inheritance |
| `{% block name %}...{% endblock %}` | Define/override a block |
| `{% include "partial.html" %}` | Include a sub-template |

**Example `templates/index.html`:**
```html
<!DOCTYPE html>
<html>
<head><title>{{ title }}</title></head>
<body>
    {% if loggedIn %}
    <p>Welcome, {{ user }}!</p>
    {% else %}
    <p>Please log in.</p>
    {% endif %}
    
    <ul>
    {% for item in items %}
        <li>{{ item }}</li>
    {% endfor %}
    </ul>
</body>
</html>
```

---

## Template Inheritance

**`templates/base.html`:**
```html
<!DOCTYPE html>
<html>
<head>
    <title>{% block title %}My App{% endblock %}</title>
    <link rel="stylesheet" href="/static/style.css">
</head>
<body>
    <nav><a href="/">Home</a> | <a href="/about">About</a></nav>
    {% block content %}{% endblock %}
    <footer>© 2025 MyApp</footer>
</body>
</html>
```

**`templates/page.html`:**
```
{% extends "base.html" %}
{% block title %}My Page{% endblock %}
{% block content %}
<h1>Hello, {{ name }}!</h1>
{% endblock %}
```

```ez
use "web"

app = WebApp()

app.get("/page", |req| {
    give render_template("templates/page.html", {"name": "Alice"})
})

app.run(8080)
```

---

## Response Helpers

### `send_file(path, contentType = nil)` → `dictionary`
Streams a file's raw bytes from C++ — the only correct way to serve binary content
(images, PDFs, downloads). Content-Type is inferred from the extension if omitted.

```ez
app.get("/logo", |req| { give send_file("assets/logo.png") })
```

### `json_response(data, status = 200)` → `dictionary`
```ez
app.get("/api", |req| { give json_response({"ok": true}, 200) })
```

### `set_cookie(res, name, value, opts = nil)` → `res`
Options: `path`, `maxAge`, `domain`, `httpOnly` (default true), `secure`, `sameSite`
(default `Lax`).

```ez
app.get("/login", |req| {
    res = { "status": 200, "body": "hi", "headers": {} }
    set_cookie(res, "token", "abc123", {"maxAge": 3600, "secure": true})
    give res
})
```

### `clear_session(req)` → `req`
Empties the session (logout).

### `secure_token(nbytes = 32)` → `string`
Cryptographically secure random hex (`BCryptGenRandom`). Used for session ids.

---

## Request Behaviour

### HEAD
A `GET` route automatically answers `HEAD` with the same headers — including the
`Content-Length` the `GET` would have produced — and no body. Register an explicit
`app.head(...)` to override. Set `app.auto_head = false` to disable.

### Trailing slashes
If `/users` is registered and `/users/` is requested (or the reverse), the request is
answered with `308 Permanent Redirect` to the registered spelling, query string
preserved. `308` rather than `301`/`302` because browsers must replay it with the same
method and body, so a `POST` is not silently downgraded to a `GET`. Set
`app.strict_slashes = true` to 404 instead.

### Multiple cookies
`Set-Cookie` may repeat. `set_cookie()` appends rather than replaces, and the session
cookie is appended alongside anything the handler set:

```ez
r = { "status": 200, "body": "ok" }
set_cookie(r, "theme", "dark")
set_cookie(r, "lang", "en")
give r          # both go out, and so does ez_session
```

Any header whose value is an array is emitted once per element.

---

## File Uploads

`multipart/form-data` parts carrying a `filename="..."` appear in `req["files"]`:

| Key | Meaning |
|---|---|
| `filename` | Client-supplied name |
| `contentType` | Part's Content-Type |
| `size` | Byte length |
| `content` | Raw content |

```ez
@app.post("/upload")
task upload(req) {
    f = req["files"]["doc"]
    writeFile("uploads/" + f["filename"], f["content"])
    give "saved " + str(f["size"]) + " bytes"
}
```

Binary uploads are byte-exact. The body reaches EZ through the FFI as a C string, so
it used to stop at the first NUL — a PNG has NULs in its `IHDR` length at byte 8, so a
PNG upload arrived 8 bytes long and every binary format was silently corrupted. The
framework now detects the short read (received length vs `Content-Length`) and pulls
the real bytes across by pointer and length, so `req["body"]`, `req["form"]` and
`req["files"]` are all built from true bytes. Text requests pay nothing for this.

### `request_body_raw(req)` → `string`
The raw body, NULs included, fetched on demand. Rarely needed — `req["body"]` is
already repaired — but useful for a handler that wants the bytes without going through
form parsing.

> Requires an interpreter with binary-mode file I/O. `readFile`/`writeFile` previously
> opened in text mode and translated LF↔CRLF, which corrupted any binary file on the
> way back out to disk.

---

## Redirects

### `redirect(url)` → `dictionary`
Returns a 302 redirect response.

```ez
use "web"

app = WebApp()

app.get("/old-path", |req| {
    give redirect("/new-path")
})

app.get("/new-path", |req| {
    give "<p>You've been redirected!</p>"
})

app.run(8080)
```

---

## Static Files

### `app.serve_static(urlPrefix, folderPath)`

Registers a route to serve static files from a directory.

```ez
use "web"

app = WebApp()

# Serve files from /static folder at /static URL prefix
app.serve_static("/static", "static")

# Now: GET /static/style.css → serves static/style.css
# GET /static/app.js → serves static/app.js
# GET /static/logo.png → serves static/logo.png

app.get("/", |req| {
    give "<html>
        <head><link rel='stylesheet' href='/static/style.css'></head>
        <body><h1>Hello!</h1><script src='/static/app.js'></script></body>
    </html>"
})

app.run(8080)
```

**Supported MIME types:**
- `.html` → `text/html`
- `.css` → `text/css`
- `.js` → `application/javascript`
- `.json` → `application/json`
- `.png` → `image/png`
- `.jpg` / `.jpeg` → `image/jpeg`
- `.gif` → `image/gif`
- `.svg` → `image/svg+xml`
- Other → `application/octet-stream`

**Security:** Directory traversal (`..`) in file names is blocked with a 403 response, as are path separators (`/`, `\`, `:`) — the `<file>` segment is percent-decoded after routing, so `%2F` would otherwise arrive here as a separator.

### Caching

Static responses carry an `ETag` (derived from the file's size and mtime) and a
`Cache-Control: public, max-age=...` header. A client that sends a matching
`If-None-Match` gets a `304 Not Modified` with no body.

```ez
app.static_max_age = 3600     # Cache-Control max-age, in seconds (default 3600)
app.static_cache_max = 128    # max files held in the in-memory cache (default 128; 0 disables)
```

The in-memory cache is keyed on the ETag, so editing a file on disk invalidates
its entry and the next request re-reads it. The cache is capped at
`static_cache_max` entries so a large directory cannot pin unbounded memory.

---

## Middleware

### `app.use(handler)`

Registers middleware. The handler receives `req` and can return a response (short-circuit) or `nil` to continue to the route handler.

```ez
use "web"

app = WebApp()

# Auth middleware
app.use(|req| {
    protected = ["/admin", "/dashboard"]
    get p in protected {
        when startsWith(req["path"], p) and not req["session"]["loggedIn"] {
            give redirect("/login")
        }
    }
    give nil   # Continue to route handler
})

# Logging middleware
app.use(|req| {
    out req["method"] + " " + req["path"]
    give nil
})

app.get("/admin", |req| {
    give "<h1>Admin Panel</h1>"
})

app.run(8080)
```

Middleware runs on **every** request, including ones that match no route. A
middleware can therefore answer a request the router would have turned into a
`404`/`405` — which is how CORS preflight is handled.

---

## CORS

### `app.useCORS(opts = nil)`

With no arguments this allows every origin (`Access-Control-Allow-Origin: *`) and
answers preflights with a permissive header set.

| option | default | meaning |
|---|---|---|
| `origins` | `"*"` | `"*"`, or an array of exact origins to allow |
| `methods` | all seven verbs | value for `Access-Control-Allow-Methods` |
| `headers` | `"Content-Type, Authorization, X-Requested-With"` | value for `Access-Control-Allow-Headers` |
| `credentials` | `false` | send `Access-Control-Allow-Credentials: true` |
| `maxAge` | `86400` | preflight cache lifetime in seconds |

```ez
app.useCORS({
    "origins": ["https://app.example.com", "https://admin.example.com"],
    "credentials": true
})
```

Notes:

- With an `origins` array, the request's `Origin` is echoed back only when it is on
  the list, and `Vary: Origin` is set so shared caches key on it. A non-matching
  origin gets **no** CORS headers at all, which is what makes the browser block it.
- `credentials: true` requires an explicit `origins` array — the CORS spec forbids
  `*` on credentialed requests, so `useCORS` throws rather than emit a header set a
  browser will reject.
- Only a real preflight (an `OPTIONS` carrying both `Origin` and
  `Access-Control-Request-Method`) is intercepted. A plain `OPTIONS` still reaches
  your own `app.options(...)` route.

---

## Security Headers

### `app.useSecurityHeaders(opts = nil)`

Adds the standard hardening headers to every response, including error responses.

| option | default | header |
|---|---|---|
| `nosniff` | `true` | `X-Content-Type-Options: nosniff` |
| `frameOptions` | `"DENY"` | `X-Frame-Options` |
| `referrer` | `"strict-origin-when-cross-origin"` | `Referrer-Policy` |
| `csp` | off | `Content-Security-Policy` |
| `permissions` | off | `Permissions-Policy` |
| `hsts` | `0` (off) | `Strict-Transport-Security: max-age=N; includeSubDomains` |

```ez
app.useSecurityHeaders({ "csp": "default-src 'self'", "hsts": 31536000 })
```

Two defaults are deliberately off. **HSTS** is meaningless over plain HTTP and, once a
browser has pinned it, a misconfigured deployment is unreachable for `max-age` seconds —
turn it on deliberately, behind TLS. **CSP** has no default that is right for every app,
and a wrong one silently breaks a working page.

This registers an `after` hook rather than middleware, because middleware runs *before*
the handler and any header it sets is lost when a handler returns its own headers.

---

## Streaming

A normal response is assembled in full before anything is sent, so the whole body must
fit in memory. `stream_*` sends it incrementally with `Transfer-Encoding: chunked` —
what server-sent events, progress feeds and larger-than-RAM exports need.

```ez
@app.get("/events")
task events(req) {
    stream_begin(req, 200, {"Content-Type": "text/event-stream", "Cache-Control": "no-cache"})
    i = 0
    while i < 10 {
        stream_write(req, "data: tick " + str(i) + "\n\n")
        i = i + 1
    }
    stream_end(req)
    give STREAMED
}
```

The handler **must** return `STREAMED`, which tells the framework the socket has already
been answered so it does not send a second response on top of the first.

---

## Byte Ranges

`serve_static` advertises `Accept-Ranges: bytes` and honours the `Range` header, so media
can seek and interrupted downloads can resume.

| Request | Response |
|---|---|
| `Range: bytes=0-499` | `206`, `Content-Range: bytes 0-499/1024` |
| `Range: bytes=500-` | `206`, from 500 to the end |
| `Range: bytes=-500` | `206`, the **last** 500 bytes |
| `Range: bytes=5000-` on a 1 KB file | `416` with `Content-Range: bytes */1024` |

Multi-range requests (`bytes=0-9,20-29`) are answered with the whole file: they require a
`multipart/byteranges` body, and every client that matters falls back gracefully when
only a single range is offered.

---

## Rate Limiting

### `app.useRateLimiter(maxReq, timeWindowMs, keyFn = nil, trustProxy = false, maxKeys = 10000)`

Allows `maxReq` requests per `timeWindowMs` per client, answering `429` beyond that.

The client key is resolved in this order: `keyFn(req)` if given, else
`X-Forwarded-For`/`X-Real-IP` when `trustProxy` is true, else `req["ip"]`.

```ez
# 100 requests per minute per IP
app.useRateLimiter(100, 60000)

# Behind nginx, keyed by the forwarded client address
app.useRateLimiter(100, 60000, nil, true)

# Keyed by API key instead of address
app.useRateLimiter(1000, 60000, |req| { give str(req["headers"]["x-api-key"]) })
```

Only enable `trustProxy` when you actually run behind a proxy — `X-Forwarded-For` is
client-supplied and trivially spoofed to evade the limit.

`maxKeys` bounds the tracking table. Buckets that fall empty are deleted, and once
the table is at the cap a previously unseen client is served but not tracked, so a
spray of unique addresses cannot grow it without bound.

---

## Custom Error Handlers

```ez
use "web"

app = WebApp()

app.on_404(|req| {
    give "<html><body>
        <h1>404 - Page Not Found</h1>
        <p>The page <code>" + req["path"] + "</code> doesn't exist.</p>
        <a href='/'>Go Home</a>
    </body></html>"
})

app.get("/", |req| { give "<h1>Home</h1>" })

app.run(8080)
```

---

## Edge Cases & Important Notes

### Request Body Size Limit
`Content-Length` is capped at **32 MB** by the C++ accelerator (`MAX_BODY_BYTES` in
`src/http_accel.cpp`). A malformed or oversized value is rejected with `400 Bad Request`;
a claimed length is never trusted.

### Concurrent Connections
Connections are accepted and served by the C++ accelerator (`http_accel.dll`), one
thread per connection, which invokes your EZ handlers through an FFI callback. The EZ
side no longer touches sockets. Concurrency is capped at `MAX_CONNECTIONS` (512):
without a ceiling, a flood spawns unbounded threads until the process dies.

**Within one process, your handlers do not run in parallel.** The connection threads are
C++, but they all call into the single EZ VM — the FFI callback marshals onto the
interpreter's main thread and blocks — so handler bodies are serialized. A handler that
blocks for one second blocks every other in-flight request for that second. Either keep
handlers short, or run multiple worker processes (below).

### Worker Processes

`workers` runs N copies of your process, each with its own VM and its own main thread,
all accepting on one shared listening socket. This is the only way handler bodies
actually run in parallel: more *threads* in one process buy nothing, because they all
queue on the same VM.

```ez
app.run(port=8080, workers=4)     # default: workers = 1
```

Measured on the same app, three concurrent 1-second handlers:

| | Elapsed |
|---|---|
| `workers=1` | 3091 ms (serialized) |
| `workers=3` | 1108 ms (parallel) |

Requests are distributed by the OS, and evenly in practice (12 requests split 4/4/4
across 3 workers). Use `worker_pid()` to see which process served a request:

```ez
app.get("/whoami", |req| { give "served by pid " + str(worker_pid()) })
```

**What stops being shared.** Each worker is a separate process with a separate heap, so
anything held in memory is per-worker and there is no shared mutable state to corrupt.
That is the point — but it means:

- **Sessions must move to a shared store.** The default `MemorySessionStore` gives each
  worker its own session table, so a user would appear logged out whenever a later
  request landed on a different worker. Use `app.setSessionStore(FileSessionStore("./sessions"))`
  or another backend shared across processes.
- **Rate limiter counters are per-worker.** With N workers the effective limit is
  roughly N × what you configured, since each keeps its own tally.
- **Any global you mutate is per-worker** — in-memory caches, counters, queues. The
  static-file cache is per-worker too, which is harmless (just N copies).
- **Your script runs once per worker**, so start-up side effects (opening files, seeding
  data, printing) happen N times. Guard anything that must happen once.

The supervisor process binds the port and then only supervises: it serves no requests.
Workers are held in a Windows job object configured to kill them when the supervisor
exits, so Ctrl-C takes the whole group down rather than orphaning workers that still hold
the port.

### Keep-Alive
HTTP/1.1 connections are kept open by default (`Connection: close` or HTTP/1.0 opts out),
so a browser fetching a page plus its assets pays for one TCP handshake instead of one
per asset. Disable with `app.keep_alive = false`. An idle connection is closed after
`IDLE_TIMEOUT_MS` (5s) while waiting for the next request.

### Compression
Responses are gzipped when the client sends `Accept-Encoding: gzip`, the body is at least
`GZIP_MIN_BYTES` (256), and the content type is compressible (text, JSON, JS, CSS, SVG —
never already-compressed image/archive types). Compressed responses carry
`Content-Encoding: gzip` and `Vary: Accept-Encoding`. zlib is linked statically, so no
extra runtime DLL is needed.

### Slow-Client (Slowloris) Defenses
Every blocking socket operation has a deadline, and — separately — receiving a whole
request has one:

| Limit | Default | Covers |
|---|---|---|
| `RECV_TIMEOUT_MS` | 15s | a single `recv()` while reading a request |
| `SEND_TIMEOUT_MS` | 15s | a single `send()` while writing a response |
| `IDLE_TIMEOUT_MS` | 5s | waiting for the next keep-alive request |
| `REQUEST_DEADLINE_MS` | 20s | the entire request, first byte to last |

The per-`recv()` timeout alone is not enough: it only bounds the gap *between* bytes, and
a client trickling one byte every 5s resets it forever while never reaching the 64KB
header cap. `REQUEST_DEADLINE_MS` bounds total wall time instead, and an over-deadline or
incomplete request is answered `408 Request Timeout` and closed. An incomplete body is
rejected rather than passed to the handler as a shorter one.

### Client IP
`req["ip"]` is the peer address from `accept()`. Behind a reverse proxy that is the
*proxy's* address — see `useRateLimiter(..., trustProxy = true)`.

### Session Storage
Sessions live in `app.sessionStore`, which defaults to `MemorySessionStore` (lost on
restart). Swap in a persistent backend:

```ez
app.setSessionStore(FileSessionStore("./sessions"))
```

Session ids are `secure_token(32)` — 256 bits from `BCryptGenRandom` — and the cookie is
set `HttpOnly; SameSite=Lax; Max-Age=<ttl>`.

### Session Expiry

Both stores take a TTL in seconds, defaulting to `SESSION_TTL_SECONDS` (86400 — one
day). A session expires that long after its last read or write.

```ez
# One hour, in memory
app.setSessionStore(MemorySessionStore(3600), 3600)

# One week, on disk
app.setSessionStore(FileSessionStore("./sessions", 604800), 604800)

# No expiry at all (records live for the life of the process)
app.setSessionStore(MemorySessionStore(0), 0)
```

The second argument to `setSessionStore` is the `Max-Age` advertised on the cookie.
Pass the same value the store was built with so the browser and the server agree on
when the session ends; pass `0` for a pure session cookie with no expiry attribute.

`MemorySessionStore` expires on access *and* sweeps the whole table on a timer, so
abandoned sessions — the common case — are reclaimed rather than accumulating for the
life of the process. `FileSessionStore` keys expiry off each file's mtime, so the
on-disk format is unchanged and existing session files stay readable.

### Binary Files
Binary responses must go through `send_file(path)` (or `serve_static`, which routes
binary content types there automatically). A binary body returned as a normal string is
truncated at its first NUL byte, because the response is handed to the FFI as a C string.

### Template Rendering Performance
The template engine is an interpreter scanning character-by-character. Deeply nested templates or very large templates may be slow. For performance-sensitive pages, pre-render to a string.

A template named by `{% extends %}` is cached in memory across renders, keyed on its
mtime, so editing a base template takes effect on the next request without a restart.

### Template Includes
`{% include %}` is bounded at `MAX_INCLUDE_DEPTH` (32) nested levels. Unlike `{% if %}`,
`{% for %}`, and `{% block %}` — which recurse into a strict substring of the template and
so always terminate — an include renders a whole file from the top, so a cycle (a template
including itself, or `a → b → a`) would otherwise recurse until the stack overflowed and
took the process down. Past the limit the render emits an `[Error: include depth
exceeded ...]` marker instead.

Include and `extends` paths are resolved relative to the template's base directory and are
**not** sandboxed to it — they are as trusted as the template source itself. Do not render
templates whose text comes from user input.

### Static File Security
`serve_static()` blocks filenames containing `..` to prevent directory traversal. However, it does not check for symlinks. Only serve static files from trusted directories.

### Windows Sockets (Winsock2)
`web` uses `WSAStartup(514)` which requests Winsock 2.2 (`514 = 0x0202`). This is always available on modern Windows. The listening socket is on `0.0.0.0` (all interfaces).

### Template `{% for %}` Scope
The `ctx` dictionary is mutated when iterating in `{% for item in items %}` — it sets `ctx[iterVar] = item` for each iteration. This means the last iterated value remains in `ctx` after the loop.

---

## Full Example: Blog Application

```ez
use "web"

posts = [
    {"id": 1, "title": "Hello EZ", "body": "Welcome to my EZ blog!", "author": "alice"},
    {"id": 2, "title": "EZ is Fast", "body": "EZ web server runs on raw sockets!", "author": "bob"}
]

app = WebApp()
app.serve_static("/static", "static")

app.get("/", |req| {
    html = "<h1>My EZ Blog</h1><ul>"
    get p in posts {
        html = html + "<li><a href='/posts/" + str(p["id"]) + "'>" + p["title"] + "</a> by " + p["author"] + "</li>"
    }
    html = html + "</ul>"
    give html
})

app.get("/posts/<id>", |req| {
    targetId = num(req["params"]["id"])
    get p in posts {
        when p["id"] == targetId {
            give "<h1>" + p["title"] + "</h1><p>By: " + p["author"] + "</p><p>" + p["body"] + "</p><a href='/'>Back</a>"
        }
    }
    give {"status": 404, "body": "<h1>Post not found</h1>", "headers": {}}
})

app.get("/api/posts", |req| {
    give posts
})

app.post("/api/posts", |req| {
    data = parse_json(req["body"])
    newPost = {
        "id": len(posts) + 1,
        "title": data["title"],
        "body": data["body"],
        "author": data["author"]
    }
    push(posts, newPost)
    give {"status": 201, "body": newPost, "headers": {}}
})

app.on_404(|req| {
    give "<h1>404 - Not Found</h1><a href='/'>Home</a>"
})

app.run(8080)
```

---

*Documentation generated from `C:\ezlib\web\main.ez` — EZ Web Framework*
