# ezweb — Web Framework for EZ

> **Version:** 2.0.2  
> **Import:** `use "web"`  
> **File:** `C:\ezlib\web\main.ez`  
> **Requires:** `kernel32.dll`, `ws2_32.dll`, `msvcrt.dll`, and the bundled `dlls/http_accel.dll` + `dlls/web_accel.dll`

---

## Overview

`ezweb` is a full-featured, Flask-inspired web framework for EZ built entirely on raw Windows Sockets (Winsock2) via FFI. It provides:

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

app = WebApp()

app.get("/", |req| {
    give "<h1>Hello from EZ Web!</h1>"
})

app.get("/api/hello", |req| {
    give {"message": "Hello, World!", "status": "ok"}
})

app.run(8080)
# → Server starts at http://localhost:8080
```

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

## File Uploads

`multipart/form-data` parts carrying a `filename="..."` appear in `req["files"]`:

| Key | Meaning |
|---|---|
| `filename` | Client-supplied name |
| `contentType` | Part's Content-Type |
| `size` | Byte length |
| `content` | Raw content |

```ez
app.post("/upload", |req| {
    f = req["files"]["doc"]
    writeFile("uploads/" + f["filename"], f["content"])
    give "saved " + str(f["size"]) + " bytes"
})
```

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
set `HttpOnly; SameSite=Lax`.

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
`ezweb` uses `WSAStartup(514)` which requests Winsock 2.2 (`514 = 0x0202`). This is always available on modern Windows. The listening socket is on `0.0.0.0` (all interfaces).

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

*Documentation generated from `E:\ezlib\ezweb\main.ez` — EZ Web Framework*
