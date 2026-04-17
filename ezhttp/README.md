# ezhttp — HTTP Client

> Production-ready HTTP client for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**ezhttp** is a full-featured HTTP client library for EZ. It provides a fluent request builder, typed response model, automatic JSON serialization, retries, timeouts, interceptors, async support, and a reusable `HTTPClient` for API integrations — all in pure EZ.

---

## Installation

```
ez install http
```

---

## Quick Start

```ez
use "http"

# Simple GET
res = hGet("https://api.example.com/users", nil)
out res.statusCode   # 200
out res.body

# Simple POST with JSON
res = hPost("https://api.example.com/users", { "name": "Alice" }, nil)
out res.isOk()       # true
out res.json()       # parsed response body
```

---

## Quick Functions

One-liner helpers for the most common cases:

```ez
# GET with query params
res = hGet("https://api.example.com/search", { "q": "ez lang", "page": "1" })

# POST JSON body
res = hPost("https://api.example.com/items", { "name": "Widget", "price": 9.99 }, nil)

# PUT
res = hPut("https://api.example.com/items/1", { "name": "Updated" }, nil)

# PATCH
res = hPatch("https://api.example.com/items/1", { "price": 4.99 }, nil)

# DELETE
res = hDelete("https://api.example.com/items/1", nil)
```

All quick functions return an `HTTPResponse` object.

---

## Request Builder

For full control, use `HTTPRequest` directly with a fluent chainable API:

```ez
use "http"

res = HTTPRequest("GET", "https://api.example.com/users")
    .param("role", "admin")
    .param("active", "true")
    .header("Authorization", "Bearer my-token")
    .timeout(10000)
    .retries(2)
    .send()
```

### Builder Methods

| Method | Description |
|---|---|
| `.header(key, value)` | Add a single request header |
| `.headers(dict)` | Add multiple headers from a dictionary |
| `.data(body)` | Set request body — auto-serializes dicts/arrays to JSON |
| `.raw(body)` | Set raw string body without serialization |
| `.param(key, value)` | Add a single URL query parameter |
| `.query(dict)` | Add multiple query parameters |
| `.timeout(ms)` | Set request timeout in milliseconds (default: `30000`) |
| `.retries(count)` | Set number of retry attempts on failure (default: `3`) |
| `.noRedirect()` | Disable automatic redirect following |
| `.send()` | Execute the request and return an `HTTPResponse` |

---

## The Response Object

All requests return an `HTTPResponse` with these fields and methods:

### Fields

| Field | Type | Description |
|---|---|---|
| `res.statusCode` | int | HTTP status code (e.g. `200`, `404`) |
| `res.body` | string | Raw response body |
| `res.headers` | dictionary | Response headers |
| `res.url` | string | Final request URL |
| `res.duration` | float | Request duration in milliseconds |
| `res.ok` | bool | `true` if status is 2xx |

### Methods

| Method | Returns | Description |
|---|---|---|
| `.isOk()` | bool | `true` if status code is 2xx |
| `.is(code)` | bool | `true` if status matches the given code |
| `.json()` | dict/array | Parses body as JSON |
| `.header(name)` | string | Gets a response header (case-insensitive) |
| `.contentType()` | string | Returns the `Content-Type` without parameters |
| `.raiseForStatus()` | self | Throws an error if status is not 2xx |

### Examples

```ez
res = hGet("https://api.example.com/users/1", nil)

# Check success
when res.isOk() {
    user = res.json()
    out user["name"]
}

# Check specific code
when res.is(HTTP_NOT_FOUND) {
    out "User not found"
}

# Get a header
out res.header("Content-Type")   # "application/json"
out res.contentType()            # "application/json"

# Throw on error
res.raiseForStatus()
```

---

## HTTPClient — Reusable API Client

`HTTPClient` is ideal for talking to a single API. Set a base URL, default headers, and timeout once — then make requests without repeating yourself.

```ez
use "http"

client = HTTPClient("https://api.example.com")
client.header("Authorization", "Bearer my-token")
client.header("Accept", "application/json")
client.timeout(15000)

# Requests are relative to the base URL
res = client.get("/users").send()
out res.json()

res = client.post("/users").data({ "name": "Bob" }).send()
out res.statusCode   # 201
```

### Client Methods

| Method | Description |
|---|---|
| `.header(key, value)` | Set a default header for all requests |
| `.timeout(ms)` | Set default timeout for all requests |
| `.onRequest(fn)` | Add a request interceptor |
| `.onResponse(fn)` | Add a response interceptor |
| `.get(path)` | Returns an `HTTPRequest` for GET |
| `.post(path)` | Returns an `HTTPRequest` for POST |
| `.put(path)` | Returns an `HTTPRequest` for PUT |
| `.patch(path)` | Returns an `HTTPRequest` for PATCH |
| `.delete(path)` | Returns an `HTTPRequest` for DELETE |
| `.head(path)` | Returns an `HTTPRequest` for HEAD |
| `.options(path)` | Returns an `HTTPRequest` for OPTIONS |

### Interceptors

Interceptors let you mutate every request or response automatically — useful for logging, auth token injection, or centralized error handling.

```ez
client = HTTPClient("https://api.example.com")

# Request interceptor — runs before every request
client.onRequest(task(req) {
    req.header("X-Request-Id", uuid())
})

# Response interceptor — runs after every response
client.onResponse(task(res) {
    when not res.isOk() {
        out "Request failed: " + str(res.statusCode)
    }
})
```

---

## Async Requests

Fire HTTP requests in the background using EZ's `spawn`:

```ez
use "http"

asyncGet("https://api.example.com/users", task(err, res) {
    when err {
        out "Error: " + err
        give
    }
    out res.json()
})

asyncPost("https://api.example.com/users", { "name": "Carol" }, task(err, res) {
    when err { out "Error: " + err }
    other { out "Created: " + str(res.statusCode) }
})
```

---

## URL Utilities

```ez
use "http"

# Build a URL with query parameters
url = buildUrl("https://api.example.com/search", {
    "q": "hello world",
    "page": "2"
})
out url
# https://api.example.com/search?q=hello%20world&page=2

# Parse a URL's query string into a dictionary
params = parseQuery("https://api.example.com/search?q=hello&page=2")
out params["q"]     # "hello"
out params["page"]  # "2"
```

---

## Status Code Constants

ezhttp exports named constants for all standard HTTP status codes:

```ez
use "http"

when res.is(HTTP_OK)                    { out "Success" }
when res.is(HTTP_CREATED)               { out "Created" }
when res.is(HTTP_NO_CONTENT)            { out "Empty response" }
when res.is(HTTP_BAD_REQUEST)           { out "Bad request" }
when res.is(HTTP_UNAUTHORIZED)          { out "Auth required" }
when res.is(HTTP_FORBIDDEN)             { out "Access denied" }
when res.is(HTTP_NOT_FOUND)             { out "Not found" }
when res.is(HTTP_TOO_MANY_REQUESTS)     { out "Rate limited" }
when res.is(HTTP_INTERNAL_SERVER_ERROR) { out "Server error" }
```

Full list: `HTTP_CONTINUE`, `HTTP_SWITCHING_PROTOCOLS`, `HTTP_OK`, `HTTP_CREATED`, `HTTP_ACCEPTED`, `HTTP_NO_CONTENT`, `HTTP_RESET_CONTENT`, `HTTP_PARTIAL_CONTENT`, `HTTP_MULTIPLE_CHOICES`, `HTTP_MOVED_PERMANENTLY`, `HTTP_FOUND`, `HTTP_SEE_OTHER`, `HTTP_NOT_MODIFIED`, `HTTP_TEMPORARY_REDIRECT`, `HTTP_PERMANENT_REDIRECT`, `HTTP_BAD_REQUEST`, `HTTP_UNAUTHORIZED`, `HTTP_PAYMENT_REQUIRED`, `HTTP_FORBIDDEN`, `HTTP_NOT_FOUND`, `HTTP_METHOD_NOT_ALLOWED`, `HTTP_NOT_ACCEPTABLE`, `HTTP_REQUEST_TIMEOUT`, `HTTP_CONFLICT`, `HTTP_GONE`, `HTTP_PAYLOAD_TOO_LARGE`, `HTTP_URI_TOO_LONG`, `HTTP_UNSUPPORTED_MEDIA_TYPE`, `HTTP_IM_A_TEAPOT`, `HTTP_UNPROCESSABLE_ENTITY`, `HTTP_TOO_MANY_REQUESTS`, `HTTP_INTERNAL_SERVER_ERROR`, `HTTP_NOT_IMPLEMENTED`, `HTTP_BAD_GATEWAY`, `HTTP_SERVICE_UNAVAILABLE`, `HTTP_GATEWAY_TIMEOUT`

---

## Complete Example — REST API Integration

```ez
use "http"

api = HTTPClient("https://jsonplaceholder.typicode.com")
api.header("Accept", "application/json")

# Fetch all posts
res = api.get("/posts").send()
posts = res.json()
out "Total posts: " + str(len(posts))

# Fetch a single post
res = api.get("/posts/1").send()
when res.isOk() {
    post = res.json()
    out post["title"]
}

# Create a new post
res = api.post("/posts")
    .data({ "title": "Hello EZ", "body": "My first post", "userId": 1 })
    .send()

res.raiseForStatus()
out "Created with ID: " + str(res.json()["id"])

# Filter with query params
res = api.get("/comments").param("postId", "1").send()
out "Comments: " + str(len(res.json()))
```

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
