# ezweb — Web Framework for EZ

> **Version:** 1.0  
> **Import:** `use "ezweb"`  
> **File:** `E:\ezlib\ezweb\main.ez`  
> **Requires:** `kernel32.dll`, `ws2_32.dll`, `msvcrt.dll`

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
use "ezweb"

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
use "ezweb"

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

### `app.delete(path, handler)`
Registers a DELETE route.

**Handler signature:** `|req| { give response }`

**Response types:**
- `string` → Sent as `text/html`
- `dictionary` / `array` → Auto-serialized to JSON (`application/json`)
- `dictionary` with `status`, `body`, `headers` keys → Full control response

```ez
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

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
use "ezweb"

app = WebApp()

app.get("/page", |req| {
    give render_template("templates/page.html", {"name": "Alice"})
})

app.run(8080)
```

---

## Redirects

### `redirect(url)` → `dictionary`
Returns a 302 redirect response.

```ez
use "ezweb"

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
use "ezweb"

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
- Other → `text/plain`

**Security:** Directory traversal (`..`) in file names is blocked with a 403 response.

---

## Middleware

### `app.use(handler)`

Registers middleware. The handler receives `req` and can return a response (short-circuit) or `nil` to continue to the route handler.

```ez
use "ezweb"

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
use "ezweb"

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
The body parser caps `Content-Length` at **10 MB** (`10485760` bytes). Larger payloads are truncated for safety.

### Concurrent Connections
Each client connection is handled in a spawned task. The server accepts connections in a `while true` loop and spawns `WebApp._handleClient` for each. This enables multiple concurrent clients.

### Session Storage
Sessions are stored in memory (`app.sessions` dictionary). Sessions are lost when the server restarts. For production, integrate with `ezdb` for persistent sessions.

### Template Rendering Performance
The template engine is an interpreter scanning character-by-character. Deeply nested templates or very large templates may be slow. For performance-sensitive pages, pre-render to a string.

### Static File Security
`serve_static()` blocks filenames containing `..` to prevent directory traversal. However, it does not check for symlinks. Only serve static files from trusted directories.

### Windows Sockets (Winsock2)
`ezweb` uses `WSAStartup(514)` which requests Winsock 2.2 (`514 = 0x0202`). This is always available on modern Windows. The listening socket is on `0.0.0.0` (all interfaces).

### Template `{% for %}` Scope
The `ctx` dictionary is mutated when iterating in `{% for item in items %}` — it sets `ctx[iterVar] = item` for each iteration. This means the last iterated value remains in `ctx` after the loop.

---

## Full Example: Blog Application

```ez
use "ezweb"

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
