# serve

A Flask-style web framework for the EZ programming language. Build HTTP servers with dynamic routing, a templating engine, middleware, sessions, static file serving, and more — all running natively over raw sockets.

---

## Installation

```bash
ez install serve
```

---

## Quick Start

```ez
use "serve"

app = WebApp()

app.get("/", |req| {
    give "<h1>Hello from EZ!</h1>"
})

app.run(3000)
```

Visit `http://localhost:3000` — that's it.

---

## Table of Contents

- [Routing](#routing)
  - [Route Parameters](#route-parameters)
  - [Query Strings](#query-strings)
- [Request Object](#request-object)
- [Responses](#responses)
- [Templates](#templates)
  - [Variables](#variables)
  - [If / Else](#if--else)
  - [For Loops](#for-loops)
  - [Include](#include)
  - [Extends & Blocks](#extends--blocks)
- [Forms & JSON](#forms--json)
- [Static Files](#static-files)
- [Redirects](#redirects)
- [Sessions & Cookies](#sessions--cookies)
- [Middleware](#middleware)
- [Error Handlers](#error-handlers)
- [Examples](#examples)

---

## Routing

Register routes with `get`, `post`, `put`, or `delete`. Each handler receives a `req` object and must return a response.

```ez
app.get("/hello", |req| {
    give "Hello, world!"
})

app.post("/submit", |req| {
    give "Form received"
})

app.put("/update", |req| {
    give "Updated"
})

app.delete("/remove", |req| {
    give "Deleted"
})
```

### Route Parameters

Wrap a segment in `<name>` to capture it as a named parameter.

```ez
app.get("/user/<id>", |req| {
    id = req["params"]["id"]
    give "User: " + id
})

app.get("/post/<slug>/comments", |req| {
    slug = req["params"]["slug"]
    give "Comments for: " + slug
})
```

### Query Strings

Query parameters from `?key=value` are available on `req["query"]`.

```ez
# GET /search?q=hello&page=2

app.get("/search", |req| {
    term = req["query"]["q"]
    page = req["query"]["page"]
    give "Searching: " + term + " (page " + page + ")"
})
```

---

## Request Object

Every handler receives a `req` dictionary with the following fields:

| Field | Type | Description |
|-------|------|-------------|
| `req["method"]` | string | HTTP method (`GET`, `POST`, etc.) |
| `req["path"]` | string | URL path (without query string) |
| `req["fullPath"]` | string | Full URL including query string |
| `req["headers"]` | dict | Request headers (keys lowercased) |
| `req["query"]` | dict | Parsed query string parameters |
| `req["form"]` | dict | Parsed form body (URL-encoded POST) |
| `req["body"]` | string | Raw request body |
| `req["params"]` | dict | URL route parameters |
| `req["cookies"]` | dict | Parsed cookies |
| `req["session"]` | dict | Session data for current user |

```ez
app.post("/login", |req| {
    username = req["form"]["username"]
    password = req["form"]["password"]
    agent    = req["headers"]["user-agent"]
    give "Hello, " + username
})
```

---

## Responses

Handlers can return several types:

### String (HTML)

```ez
app.get("/", |req| {
    give "<h1>Welcome</h1>"
})
```

### Dictionary (full control)

```ez
app.get("/custom", |req| {
    give {
        "status": 200,
        "body": "<p>Custom response</p>",
        "headers": {
            "Content-Type": "text/html",
            "X-Custom-Header": "value"
        }
    }
})
```

### JSON (auto-serialized)

Return a dictionary or array without a `"body"` key and it is automatically serialized as JSON with the correct `Content-Type`.

```ez
app.get("/api/data", |req| {
    give {
        "users": ["Alice", "Bob"],
        "count": 2
    }
})
```

### HTTP Status Codes

```ez
app.get("/secret", |req| {
    give {"status": 401, "body": "Unauthorized"}
})
```

Supported status codes: `200`, `301`, `302`, `400`, `401`, `403`, `404`, `500`.

---

## Templates

Use `render_template(path, ctx)` to render an HTML file with dynamic data.

```ez
use "serve"

app = WebApp()

app.get("/profile/<name>", |req| {
    give render_template("views/profile.html", {
        "name": req["params"]["name"],
        "loggedIn": true
    })
})

app.run(3000)
```

### Variables

```html
<p>Hello, {{ name }}!</p>
<p>Email: {{ user.email }}</p>
```

Dot notation resolves nested keys: `{{ user.address.city }}`.

### If / Else

```html
{% if loggedIn %}
  <p>Welcome back!</p>
{% else %}
  <p>Please log in.</p>
{% endif %}
```

Falsy values: `false`, `0`, empty string, empty array/dictionary, `nil`.

### For Loops

Iterate over arrays or dictionaries.

```html
<ul>
{% for item in items %}
  <li>{{ item }}</li>
{% endfor %}
</ul>
```

### Include

Insert another template file inline.

```html
{% include "partials/nav.html" %}
<main>...</main>
{% include "partials/footer.html" %}
```

### Extends & Blocks

Define a base layout and override named blocks in child templates.

**`views/base.html`**
```html
<!DOCTYPE html>
<html>
<head>
  <title>{% block title %}My App{% endblock %}</title>
</head>
<body>
  {% block content %}{% endblock %}
</body>
</html>
```

**`views/home.html`**
```html
{% extends "base.html" %}

{% block title %}Home{% endblock %}

{% block content %}
  <h1>Welcome, {{ name }}!</h1>
{% endblock %}
```

---

## Forms & JSON

### URL-Encoded Forms

HTML form submissions (`application/x-www-form-urlencoded`) are automatically parsed into `req["form"]`.

```ez
app.post("/register", |req| {
    name  = req["form"]["name"]
    email = req["form"]["email"]
    give render_template("views/success.html", {"name": name})
})
```

```html
<form method="POST" action="/register">
  <input name="name" />
  <input name="email" />
  <button type="submit">Register</button>
</form>
```

### Raw Body / JSON

For JSON APIs, read `req["body"]` directly.

```ez
app.post("/api/create", |req| {
    data = from_json(req["body"])
    title = data["title"]
    give {"status": 201, "body": {"created": title}}
})
```

---

## Static Files

Serve a folder of static assets under a URL prefix.

```ez
app.static("/static", "public")
```

Files in `public/` are now accessible at `/static/<filename>`.

```html
<link rel="stylesheet" href="/static/style.css">
<script src="/static/app.js"></script>
<img src="/static/logo.png">
```

Supported MIME types: `.html`, `.css`, `.js`, `.json`, `.png`, `.jpg`, `.jpeg`, `.gif`, `.svg`.

Directory traversal (`../`) is blocked automatically.

---

## Redirects

Use the built-in `redirect(url)` helper to return a `302` response.

```ez
app.get("/old-page", |req| {
    give redirect("/new-page")
})

app.post("/login", |req| {
    # ... validate credentials ...
    give redirect("/dashboard")
})
```

---

## Sessions & Cookies

Session data is stored server-side and tied to a cookie named `ez_session`. Read and write through `req["session"]`.

```ez
app.post("/login", |req| {
    username = req["form"]["username"]
    req["session"]["user"] = username
    req["session"]["loggedIn"] = true
    give redirect("/dashboard")
})

app.get("/dashboard", |req| {
    when not req["session"]["loggedIn"] {
        give redirect("/login")
    }
    give render_template("views/dashboard.html", {
        "user": req["session"]["user"]
    })
})

app.get("/logout", |req| {
    req["session"] = {}
    give redirect("/")
})
```

### Cookies

Incoming cookies are available on `req["cookies"]`.

```ez
app.get("/", |req| {
    theme = req["cookies"]["theme"]
    give render_template("views/index.html", {"theme": theme})
})
```

---

## Middleware

Middleware runs before every request handler. Return a response to short-circuit the handler, or return `nil` to continue.

```ez
# Logging middleware
app.use(|req| {
    out req["method"] + " " + req["path"]
    give nil  # Continue to handler
})

# Auth guard
app.use(|req| {
    when not req["session"]["loggedIn"] and req["path"] != "/login" {
        give redirect("/login")  # Short-circuit
    }
    give nil
})
```

Multiple middleware runs in registration order. The first one to return a non-nil response wins.

---

## Error Handlers

### 404 Not Found

```ez
app.on_404(|req| {
    give render_template("views/404.html", {
        "path": req["path"]
    })
})
```

### 500 Internal Server Error

```ez
app.on_500(|req| {
    give "<h1>Something went wrong.</h1>"
})
```

---

## Examples

### JSON API

```ez
use "serve"

app = WebApp()

users = [
    {"id": 1, "name": "Ali",   "city": "Karachi"},
    {"id": 2, "name": "Usman", "city": "Lahore"}
]

app.get("/api/users", |req| {
    give users
})

app.get("/api/users/<id>", |req| {
    id = num(req["params"]["id"])
    get u in users {
        when u["id"] == id { give u }
    }
    give {"status": 404, "body": {"error": "User not found"}}
})

app.post("/api/users", |req| {
    data = from_json(req["body"])
    push(users, data)
    give {"status": 201, "body": data}
})

app.run(4000)
```

---

### Login & Session

```ez
use "serve"

app = WebApp()
app.static("/static", "public")

app.get("/", |req| {
    when req["session"]["user"] {
        give redirect("/dashboard")
    }
    give redirect("/login")
})

app.get("/login", |req| {
    give render_template("views/login.html", {})
})

app.post("/login", |req| {
    username = req["form"]["username"]
    password = req["form"]["password"]

    when username == "admin" and password == "secret" {
        req["session"]["user"] = username
        give redirect("/dashboard")
    }

    give render_template("views/login.html", {
        "error": "Invalid credentials"
    })
})

app.get("/dashboard", |req| {
    when not req["session"]["user"] {
        give redirect("/login")
    }
    give render_template("views/dashboard.html", {
        "user": req["session"]["user"]
    })
})

app.get("/logout", |req| {
    req["session"] = {}
    give redirect("/login")
})

app.on_404(|req| {
    give render_template("views/404.html", {"path": req["path"]})
})

app.run(5000)
```

---

### Template with Layout

**`views/base.html`**
```html
<!DOCTYPE html>
<html>
<head>
  <title>{% block title %}App{% endblock %}</title>
  <link rel="stylesheet" href="/static/style.css">
</head>
<body>
  {% include "views/partials/nav.html" %}
  <main>
    {% block content %}{% endblock %}
  </main>
</body>
</html>
```

**`views/index.html`**
```html
{% extends "views/base.html" %}

{% block title %}Home{% endblock %}

{% block content %}
<h1>Hello, {{ user }}!</h1>
{% if items %}
<ul>
{% for item in items %}
  <li>{{ item }}</li>
{% endfor %}
</ul>
{% else %}
<p>No items found.</p>
{% endif %}
{% endblock %}
```

**`app.ez`**
```ez
use "serve"

app = WebApp()
app.static("/static", "public")

app.get("/", |req| {
    give render_template("views/index.html", {
        "user": "Abdullah",
        "items": ["Dashboard", "Settings", "Profile"]
    })
})

app.run(3000)
```

---

## Common Issues

**Port already in use** — Another process is using that port. Change the port number in `app.run(port)`.

**404 on static files** — Make sure the folder path passed to `app.static()` exists and matches the actual directory on disk.

**Template not rendering** — Verify the file path passed to `render_template` is correct relative to where you run the script. Use forward slashes.

**Form values empty** — Ensure the HTML form uses `method="POST"` and inputs have a `name` attribute.

**Session not persisting** — Sessions are in-memory only. Restarting the server clears all sessions.

---

## Notes

- The server is multi-threaded — each request is handled in a separate thread via `spawn`.
- Body size is capped at **10 MB** per request.
- Sessions are stored in memory and do not persist across restarts.
- Static file serving blocks `..` path segments to prevent directory traversal.

---

*MIT License — Free for any use*
