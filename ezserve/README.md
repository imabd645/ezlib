# serve

> Production-hardened native HTTP web framework for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**serve** is a pure-EZ web framework built directly on raw WinSock2 FFI bindings — no C++ networking dependencies, no external libraries. It provides an Express.js-style API with built-in DDoS protection, Slowloris mitigation, backpressure limits, and RFC-compliant HTTP responses.

---

## Installation

```
ez install serve
```

---

## Quick Start

```ez
use "serve"

app = App()

app.get("/", task(req) {
    give { "body": "<h1>Hello from EZ!</h1>" }
})

app.get("/ping", task(req) {
    give { "status": 200, "body": { "message": "pong" } }
})

app.listen(3000)
```

```
🚀 Hardened Native EZ Server listening on http://localhost:3000
```

---

## Routing

Register routes using HTTP method helpers. Handlers receive a `req` object and return a response.

```ez
app.get("/users", task(req) {
    give { "body": [{ "id": 1, "name": "Alice" }] }
})

app.post("/users", task(req) {
    data = from_json(req["body"])
    give { "status": 201, "body": { "created": data["name"] } }
})

app.put("/users", task(req) {
    give { "status": 200, "body": "Updated" }
})

app.delete("/users", task(req) {
    give { "status": 204, "body": "" }
})
```

### Supported Methods

| Method | Registration |
|---|---|
| GET | `app.get(path, handler)` |
| POST | `app.post(path, handler)` |
| PUT | `app.put(path, handler)` |
| DELETE | `app.delete(path, handler)` |

---

## The Request Object

Every handler receives a `req` dictionary with the following fields:

| Field | Type | Description |
|---|---|---|
| `req["method"]` | string | HTTP method (`"GET"`, `"POST"`, etc.) |
| `req["path"]` | string | URL path without query string (e.g. `"/users"`) |
| `req["fullPath"]` | string | Full raw URL including query string |
| `req["headers"]` | dictionary | Parsed request headers (lowercase keys) |
| `req["query"]` | dictionary | Parsed and URL-decoded query parameters |
| `req["form"]` | dictionary | Parsed `application/x-www-form-urlencoded` body |
| `req["body"]` | string | Raw request body |

### Query Parameters

```ez
# GET /search?q=hello+world&page=2
app.get("/search", task(req) {
    q    = req["query"]["q"]     # "hello world"
    page = req["query"]["page"]  # "2"
    give { "body": { "query": q, "page": page } }
})
```

### Form Data

```ez
# POST /login  (Content-Type: application/x-www-form-urlencoded)
app.post("/login", task(req) {
    username = req["form"]["username"]
    password = req["form"]["password"]
    give { "body": { "user": username } }
})
```

### JSON Body

```ez
# POST /api/data  (Content-Type: application/json)
app.post("/api/data", task(req) {
    payload = from_json(req["body"])
    give { "status": 201, "body": payload }
})
```

### Reading Headers

```ez
app.get("/whoami", task(req) {
    agent = req["headers"]["user-agent"]
    give { "body": agent }
})
```

---

## Responses

A handler can return a **string**, **array**, **dictionary**, or a **full response object**.

### Plain String (HTML)

```ez
app.get("/", task(req) {
    give "<h1>Welcome</h1>"
})
```

### JSON (automatic serialization)

```ez
app.get("/users", task(req) {
    give [{ "id": 1 }, { "id": 2 }]
})
```

Returning an array or dictionary automatically sets `Content-Type: application/json`.

### Full Response Object

```ez
app.post("/items", task(req) {
    give {
        "status": 201,
        "headers": { "X-Custom-Header": "value" },
        "body": { "created": true }
    }
})
```

### Supported Status Codes

| Code | Phrase |
|---|---|
| `200` | OK |
| `201` | Created |
| `202` | Accepted |
| `204` | No Content |
| `301` | Moved Permanently |
| `302` | Found |
| `304` | Not Modified |
| `400` | Bad Request |
| `401` | Unauthorized |
| `403` | Forbidden |
| `404` | Not Found |
| `405` | Method Not Allowed |
| `418` | I'm a teapot |
| `429` | Too Many Requests |
| `500` | Internal Server Error |
| `502` | Bad Gateway |
| `503` | Service Unavailable |

---

## Static Files

Serve a directory of static files under a URL prefix:

```ez
app.serveStatic("/static", "C:\\myapp\\public")
app.listen(8080)
```

A request to `/static/style.css` will serve `C:\myapp\public\style.css`.

**Auto-detected MIME types:** `.html`, `.htm`, `.css`, `.js`, `.json`, `.png`, `.jpg`, `.jpeg`, `.svg` — falls back to `text/plain`.

---

## Security Features

ezserve includes production hardening out of the box — no configuration required.

### Backpressure / Connection Limiting
Concurrent connections are capped at **1,024**. Requests beyond this receive an immediate `503 Service Unavailable`, preventing memory exhaustion under heavy load.

### Slowloris Mitigation
A **5-second receive timeout** (`SO_RCVTIMEO`) is set on every accepted socket. Clients that hold connections open without sending data are dropped automatically.

### Payload Size Cap
Request bodies are hard-capped at **10 MB**. Any `Content-Length` declaring a larger payload is ignored, preventing memory exhaustion attacks.

### RFC-Compliant Status Lines
All responses include a proper status phrase (e.g. `HTTP/1.1 404 Not Found`).

### Default Security Headers
Every response automatically includes:
```
Server: EZ-Native Framework
Connection: close
```

---

## How It Works

ezserve calls Windows networking APIs directly through FFI — no middleware, no runtime dependencies:

| Win32 API | Purpose |
|---|---|
| `WSAStartup` / `WSACleanup` | Initialize and shut down WinSock |
| `socket` | Create a TCP socket (`AF_INET`, `SOCK_STREAM`) |
| `bind` / `listen` / `accept` | Bind to port and accept incoming connections |
| `setsockopt` | Configure per-socket receive timeouts |
| `recv` / `send` | Read and write raw byte buffers |
| `closesocket` | Close the connection |

Each accepted connection is dispatched via EZ's `spawn()` for concurrent processing.

---

## Requirements

- **Platform**: Windows only (uses `ws2_32.dll`, `msvcrt.dll`)
- **EZ**: Any version supporting `os_load_lib`, `os_call`, and `spawn`

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
