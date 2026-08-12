# `serve` — Web Framework for EZ

> Build web servers and APIs in EZ with a familiar Express-style API.
> Native multithreaded. Zero dependencies. Pure EZ.

---

## Overview

`serve` is EZ's built-in web framework, modelled after Express.js and Flask. Define routes, serve static files, and return HTML or JSON — all backed by EZ's native multithreaded `server()` engine for real concurrency without configuration.

```ez
use "serve"

app = App()

app.get("/", |req| {
    give "<h1>Hello from EZ!</h1>"
})

app.get("/ping", |req| {
    give { "status": "ok", "uptime": os.uptime() }
})

app.listen(3000)
```

---

## Getting Started

### Installation

`serve` is part of the EZ standard library:

```ez
use "serve"
```

### Minimal Server

```ez
use "serve"

app = App()

app.get("/", |req| {
    give "Welcome to my EZ server!"
})

app.listen(8080)
```

Run it:

```bash
./ez server.ez
# 🚀 EZ Web Framework listening on http://localhost:8080
```

---

## Routing

Register routes by HTTP method. Each handler receives a `req` object and returns a response.

### `app.get(path, handler)`
### `app.post(path, handler)`
### `app.put(path, handler)`
### `app.delete(path, handler)`

```ez
app.get("/hello", |req| {
    give "Hello, world!"
})

app.post("/submit", |req| {
    give { "received": true }
})

app.put("/update", |req| {
    give { "updated": true }
})

app.delete("/item", |req| {
    give { "deleted": true }
})
```

---

## The Request Object

Every route handler receives a `req` object with the following properties:

| Property | Type | Description |
|----------|------|-------------|
| `req.method` | `string` | HTTP method (`GET`, `POST`, etc.) |
| `req.path` | `string` | Request path (e.g. `/api/users`) |

---

## Responses

Route handlers return a value. The framework automatically detects the type and sets appropriate headers.

### Return HTML (string)

```ez
app.get("/", |req| {
    give "<h1>Welcome</h1><p>Built with EZ.</p>"
})
```
→ `Content-Type: text/html`, status `200`

---

### Return JSON (dictionary or array)

```ez
app.get("/user", |req| {
    give { "id": 1, "name": "Alice", "active": true }
})

app.get("/users", |req| {
    give [
        { "id": 1, "name": "Alice" },
        { "id": 2, "name": "Bob" }
    ]
})
```
→ `Content-Type: application/json`, status `200`, body auto-serialized

---

### Return a Custom Response

Return a dictionary with `status`, `headers`, and `body` for full control:

```ez
app.get("/teapot", |req| {
    give {
        "status": 418,
        "headers": { "Content-Type": "text/plain" },
        "body": "I'm a teapot."
    }
})
```

If the returned dictionary contains a `status` key, the framework passes it through as-is — no wrapping.

---

### Response Type Summary

| Return value | Status | Content-Type |
|--------------|--------|-------------|
| `string` | 200 | `text/html` |
| `dictionary` (no `status` key) | 200 | `application/json` |
| `array` | 200 | `application/json` |
| `dictionary` with `status` key | as specified | as specified |

---

## Static Files

Serve an entire directory of static files under a URL prefix using `serveStatic`:

```ez
app.serveStatic("/static", "public")
```

Any request to `/static/...` will resolve to a file inside the `public/` directory:

```
GET /static/style.css     → public/style.css
GET /static/js/app.js     → public/js/app.js
GET /static/img/logo.png  → public/img/logo.png
```

Files are served using `serveFile()` which sets appropriate MIME types. If the file does not exist, routing falls through to a 404.

---

## 404 Handling

Unmatched routes automatically return a formatted HTML 404 page — no configuration needed:

```html
<h1>404 Not Found</h1>
<p>The route <strong>/missing</strong> does not exist on this EZ server.</p>
```

---

## `app.listen(port)`

Starts the server on the given port. This call is **blocking** — it runs until the process is terminated.

```ez
app.listen(3000)
```

The server is **multithreaded** by default via EZ's native `server()` engine, handling concurrent requests without any additional setup.

---

## Examples

### JSON API Server

```ez
use "serve"

app = App()

users = [
    { "id": 1, "name": "Alice" },
    { "id": 2, "name": "Bob" }
]

app.get("/api/users", |req| {
    give users
})

app.get("/api/health", |req| {
    give { "status": "ok" }
})

app.listen(4000)
```

### Serving an HTML Front-End + API

```ez
use "serve"

app = App()

# Serve static front-end files
app.serveStatic("/", "public")

# API routes
app.get("/api/status", |req| {
    give { "alive": true, "version": "1.0" }
})

app.post("/api/echo", |req| {
    give { "echo": "received" }
})

app.listen(8080)
```

### Custom Error Responses

```ez
app.get("/secret", |req| {
    give {
        "status": 403,
        "headers": { "Content-Type": "application/json" },
        "body": to_json({ "error": "Forbidden", "message": "You shall not pass." })
    }
})
```

### Serving Dynamic HTML

```ez
app.get("/dashboard", |req| {
    html = "<html><body>"
         + "<h1>Dashboard</h1>"
         + "<p>Server has been running for " + str(os.uptime()) + " seconds.</p>"
         + "</body></html>"
    give html
})
```

### File Download Endpoint

```ez
app.get("/download", |req| {
    when fs.exists("report.pdf") {
        give {
            "status": 200,
            "headers": {
                "Content-Type": "application/pdf",
                "Content-Disposition": "attachment; filename=\"report.pdf\""
            },
            "body": os.readFile("report.pdf")
        }
    }
    give { "status": 404, "headers": { "Content-Type": "text/plain" }, "body": "File not found" }
})
```

---

## Architecture

```
app.listen(port)
  └── server(port, handler)        ← Native multithreaded EZ server
        └── For each request:
              1. Match exact route  (method + path)
              2. Check static paths (prefix match → file lookup)
              3. Return 404
```

Route matching is performed in order: explicit routes always take priority over static file paths.

---

## Notes

- `app.listen()` is **blocking**. Place it as the last call in your program.
- Routes are matched **exactly** — there is no wildcard or parameter matching (e.g. `/user/:id`) in the current version.
- Static file serving strips the route prefix and maps the remainder to the target directory. A request for `/static/a/b.css` with `serveStatic("/static", "public")` resolves to `public/a/b.css`.
- Returning a plain dictionary without a `status` key always yields a `200 application/json` response.

---

## License

Part of the EZ standard library — MIT License. See [LICENSE](./LICENSE).

---

*Routes registered. Server running. Ship it.*
