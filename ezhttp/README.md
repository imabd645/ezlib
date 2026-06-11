# ezhttp — HTTP Client Library for EZ

> **Version:** 2.0  
> **Import:** `use "ezhttp"`  
> **File:** `E:\ezlib\ezhttp\main.ez`

---

## Overview

`ezhttp` is a production-ready HTTP client for EZ, providing a fluent request builder API with retries, timeouts, interceptors, and query parameter encoding. It wraps EZ's built-in `http_get` and `http_post` primitives.

After import, all functionality is available via:
- `http` — Main namespace dictionary
- `HTTPRequest`, `HTTPResponse`, `HTTPClient` — Model classes
- `hGet`, `hPost`, `hPut`, `hDelete`, `hPatch` — Global shorthands
- `json` — JSON parse/stringify helpers
- HTTP status code constants (`HTTP_OK`, `HTTP_NOT_FOUND`, etc.)

---

## Quick Start

```ez
use "ezhttp"

# Simple GET
response = httpGet("https://api.github.com/users/octocat", nil)
out response.statusCode   # → 200
out response.body         # → JSON string

# POST JSON
data = {"name": "Alice", "age": 30}
response = httpPost("https://api.example.com/users", data, nil)
out response.statusCode   # → 200 or 201
```

---

## HTTP Status Code Constants

```ez
use "ezhttp"

out HTTP_OK               # → 200
out HTTP_CREATED          # → 201
out HTTP_BAD_REQUEST      # → 400
out HTTP_UNAUTHORIZED     # → 401
out HTTP_FORBIDDEN        # → 403
out HTTP_NOT_FOUND        # → 404
out HTTP_INTERNAL_SERVER_ERROR  # → 500
```

Full list of exported constants:
`HTTP_CONTINUE`, `HTTP_OK`, `HTTP_CREATED`, `HTTP_ACCEPTED`, `HTTP_NO_CONTENT`, `HTTP_RESET_CONTENT`, `HTTP_PARTIAL_CONTENT`, `HTTP_MULTIPLE_CHOICES`, `HTTP_MOVED_PERMANENTLY`, `HTTP_FOUND`, `HTTP_SEE_OTHER`, `HTTP_NOT_MODIFIED`, `HTTP_TEMPORARY_REDIRECT`, `HTTP_PERMANENT_REDIRECT`, `HTTP_BAD_REQUEST`, `HTTP_UNAUTHORIZED`, `HTTP_FORBIDDEN`, `HTTP_NOT_FOUND`, `HTTP_METHOD_NOT_ALLOWED`, `HTTP_CONFLICT`, `HTTP_GONE`, `HTTP_UNPROCESSABLE_ENTITY`, `HTTP_TOO_MANY_REQUESTS`, `HTTP_INTERNAL_SERVER_ERROR`, `HTTP_BAD_GATEWAY`, `HTTP_SERVICE_UNAVAILABLE`, `HTTP_GATEWAY_TIMEOUT`.

---

## Model: `HTTPResponse`

All request functions return an `HTTPResponse` object.

### Properties
| Property | Type | Description |
|---|---|---|
| `statusCode` | `number` | HTTP status code (e.g. 200, 404) |
| `body` | `string` | Raw response body |
| `headers` | `dictionary` | Response headers (may be empty) |
| `url` | `string` | The URL that was requested |
| `duration` | `number` | Time in milliseconds |
| `ok` | `boolean` | `true` if status 200–299 |

### `response.isOk()` → `boolean`
Returns `true` if `statusCode` is 200–299.

### `response.is(code)` → `boolean`
Returns `true` if `statusCode == code`.

### `response.json()` → `dictionary | array | nil`
Parses the body as JSON. Returns `nil` if body is empty.

```ez
use "ezhttp"

res = httpGet("https://httpbin.org/get", nil)
data = res.json()
out data["url"]   # → "https://httpbin.org/get"
```

### `response.header(name)` → `string | nil`
Gets a response header by name (case-insensitive).

```ez
use "ezhttp"

res = httpGet("https://example.com", nil)
ct = res.header("Content-Type")
out ct
```

### `response.contentType()` → `string | nil`
Returns the Content-Type header without parameters (e.g. `"application/json"` not `"application/json; charset=utf-8"`).

### `response.raiseForStatus()` → `self`
Throws `"HTTP {code}: {body}"` if status is not 2xx.

```ez
use "ezhttp"

try {
    res = httpGet("https://api.example.com/protected", nil)
    res.raiseForStatus()
    out res.json()
} catch e {
    out "Request failed: " + e  # → "HTTP 401: Unauthorized"
}
```

---

## Model: `HTTPRequest`

A fluent request builder. Created manually or via `HTTPClient`.

### `HTTPRequest(method, url)`
Creates a request.

```ez
use "ezhttp"

req = HTTPRequest("GET", "https://example.com")
req = HTTPRequest("POST", "https://api.example.com/data")
```

### `req.header(key, value)` → `self`
Adds a single header.

```ez
use "ezhttp"

req = HTTPRequest("GET", "https://api.example.com")
req.header("Authorization", "Bearer mytoken123")
req.header("Accept", "application/json")
res = req.send()
```

### `req.headers(dict)` → `self`
Adds multiple headers from a dictionary.

```ez
use "ezhttp"

req = HTTPRequest("POST", "https://api.example.com/data")
req.headers({
    "Authorization": "Bearer token",
    "Content-Type": "application/json",
    "X-Request-ID": "abc-123"
})
```

### `req.data(body)` → `self`
Sets the request body. Dictionaries/arrays are auto-serialized to JSON with `Content-Type: application/json`.

```ez
use "ezhttp"

# JSON body (auto-serialized)
req = HTTPRequest("POST", "https://api.example.com/users")
req.data({"name": "Alice", "age": 30})

# String body
req2 = HTTPRequest("POST", "https://api.example.com/raw")
req2.data("plain text body")
```

### `req.raw(body)` → `self`
Sets the request body as-is (no auto-JSON-serialization).

### `req.param(key, value)` → `self`
Adds a single query parameter (URL-encoded).

### `req.query(dict)` → `self`
Adds multiple query parameters.

```ez
use "ezhttp"

req = HTTPRequest("GET", "https://api.github.com/search/repositories")
req.query({"q": "EZ language", "sort": "stars", "per_page": 10})
res = req.send()
# → GET https://api.github.com/search/repositories?q=EZ%20language&sort=stars&per_page=10
```

### `req.timeout(ms)` → `self`
Sets the timeout in milliseconds (default: 30000).

### `req.retries(count)` → `self`
Sets the number of retry attempts on failure (default: 3).

### `req.noRedirect()` → `self`
Disables redirect following.

### `req.send()` → `HTTPResponse`
Executes the request and returns an `HTTPResponse`.

```ez
use "ezhttp"

res = HTTPRequest("GET", "https://httpbin.org/get")
    .header("User-Agent", "EZApp/1.0")
    .param("format", "json")
    .send()

out res.statusCode
out res.json()
```

---

## Quick Functions

### `httpGet(url, params)` → `HTTPResponse`
Quick GET with optional query parameters dictionary.

```ez
use "ezhttp"

# Simple GET
res = httpGet("https://api.example.com/users", nil)

# GET with query params
res = httpGet("https://api.example.com/search", {
    "query": "hello",
    "page": 1,
    "limit": 20
})

out res.json()
```

---

### `httpPost(url, body, headers)` → `HTTPResponse`
Quick POST.

```ez
use "ezhttp"

# POST JSON body
res = httpPost("https://api.example.com/users", {
    "name": "Bob",
    "email": "bob@example.com"
}, nil)

# POST with custom headers
res = httpPost(
    "https://api.example.com/data",
    "raw-text-data",
    {"Authorization": "Bearer token", "Content-Type": "text/plain"}
)
```

---

### `httpPut(url, body, headers)` → `HTTPResponse`
Quick PUT.

```ez
use "ezhttp"

res = httpPut("https://api.example.com/users/1", {"name": "Alice Updated"}, nil)
```

---

### `httpPatch(url, body, headers)` → `HTTPResponse`
Quick PATCH.

```ez
use "ezhttp"

res = httpPatch("https://api.example.com/users/1", {"email": "new@example.com"}, nil)
```

---

### `httpDelete(url, headers)` → `HTTPResponse`
Quick DELETE.

```ez
use "ezhttp"

res = httpDelete("https://api.example.com/users/1", nil)
out res.statusCode  # → 200 or 204
```

---

## Model: `HTTPClient`

A base-URL client with default headers, timeout, and interceptors. Use this for APIs where you want to share a base URL and auth token.

### `HTTPClient(baseUrl)`
Creates a client with a base URL.

```ez
use "ezhttp"

client = HTTPClient("https://api.example.com")
client.header("Authorization", "Bearer my-api-key")
client.timeout(10000)   # 10 second timeout
```

### `client.header(key, value)` → `self`
Sets a default header applied to all requests.

### `client.timeout(ms)` → `self`
Sets the default timeout.

### `client.onRequest(fn)` → `self`
Adds a request interceptor. The function receives the `HTTPRequest` and can modify it.

```ez
use "ezhttp"

client = HTTPClient("https://api.example.com")
client.onRequest(|req| {
    # Add timestamp to all requests
    req.header("X-Timestamp", str(clock()))
})
```

### `client.onResponse(fn)` → `self`
Adds a response interceptor.

### `client.get(path)` → `HTTPRequest`
Creates a GET request relative to the base URL. Returns an `HTTPRequest` — call `.send()` to execute.

### `client.post(path)` → `HTTPRequest`
Creates a POST request.

### `client.put(path)` / `client.patch(path)` / `client.delete(path)` / `client.head(path)` / `client.options(path)` → `HTTPRequest`

```ez
use "ezhttp"

client = HTTPClient("https://api.github.com")
client.header("Authorization", "Bearer ghp_abc123")
client.header("Accept", "application/vnd.github.v3+json")

# GET user
userRes = client.get("/users/octocat").send()
out userRes.json()["public_repos"]

# GET repos
reposRes = client.get("/users/octocat/repos")
    .query({"sort": "stars", "per_page": 5})
    .send()
out len(reposRes.json())   # → 5
```

---

## URL Utilities

### `buildUrl(base, params)` → `string`
Builds a URL with query parameters (URL-encoded).

```ez
use "ezhttp"

url = buildUrl("https://search.example.com", {
    "q": "hello world",
    "lang": "en",
    "page": 2
})
out url
# → "https://search.example.com?q=hello%20world&lang=en&page=2"
```

---

### `parseQuery(url)` → `dictionary`
Parses query string parameters from a URL.

```ez
use "ezhttp"

params = parseQuery("https://example.com/search?q=hello&page=2&limit=20")
out params["q"]      # → "hello"
out params["page"]   # → "2"
out params["limit"]  # → "20"
```

---

## JSON Helpers

```ez
use "ezhttp"

# Parse JSON
data = json.parse("{\"name\": \"Alice\", \"age\": 30}")
out data["name"]  # → "Alice"

# Stringify to JSON
str = json.stringify({"x": 1, "y": 2})
out str  # → '{"x":1,"y":2}'
```

---

## Async HTTP

### `asyncGet(url, callback)`
Spawns a GET request in the background.

```ez
use "ezhttp"

asyncGet("https://api.example.com/data", |err, res| {
    when err {
        out "Error: " + err
    } other {
        out "Status: " + str(res.statusCode)
    }
})

out "Request sent in background..."
```

### `asyncPost(url, body, callback)`
Spawns a POST request in the background.

---

## Edge Cases & Important Notes

### Status Code Limitations
The EZ built-ins `http_get` and `http_post` return only the response body — they do NOT return HTTP status codes. `ezhttp` defaults `statusCode` to `200` for all successful calls. For APIs that return error info in JSON, it attempts to read a `status` or `statusCode` field from the response body.

This means: `response.isOk()` may return `true` even for logical errors if the server returns a 200 with `{"status": "error"}`.

### PUT/PATCH/DELETE via Method Override
Since EZ's native HTTP only supports GET and POST, `PUT`, `PATCH`, `DELETE`, `HEAD`, and `OPTIONS` are sent as POST with the `X-HTTP-Method-Override` header. Many servers support this pattern, but some may not.

For full HTTP method support, use `ezdb` (the `ezai` library internally uses `http_post` directly) or the curl-based `ezemail` pattern.

### Retry on Network Errors
The retry loop only catches thrown errors (network failures). It does NOT retry on HTTP 5xx responses — only on exceptions from the underlying `http_get` / `http_post` calls.

### Query Parameter Encoding
Parameters are URL-encoded via the EZ built-in `url_encode()`. Spaces become `%20`, special chars are properly encoded.

---

## Full Example: REST API Client

```ez
use "ezhttp"

# GitHub API client
github = HTTPClient("https://api.github.com")
github.header("Accept", "application/vnd.github.v3+json")
github.header("User-Agent", "EZ-App/1.0")

task getUser(username) {
    res = github.get("/users/" + username).send()
    when not res.isOk() { give nil }
    data = res.json()
    give {
        "name": data["name"],
        "repos": data["public_repos"],
        "followers": data["followers"]
    }
}

task searchRepos(query, lang) {
    res = github.get("/search/repositories")
        .query({"q": query + " language:" + lang, "sort": "stars", "per_page": 5})
        .send()
    when not res.isOk() { give [] }
    data = res.json()
    give data["items"]
}

# Get user info
user = getUser("torvalds")
out "Name: " + user["name"]
out "Public Repos: " + str(user["repos"])
out "Followers: " + str(user["followers"])

# Search repos
repos = searchRepos("kernel", "c")
out "\nTop C kernel repos:"
get r in repos {
    out "  " + r["full_name"] + " ⭐ " + str(r["stargazers_count"])
}
```

---

## Full Example: Webhook Sender

```ez
use "ezhttp"

task sendWebhook(url, event, data) {
    payload = {
        "event": event,
        "timestamp": str(clock()),
        "data": data
    }
    
    req = HTTPRequest("POST", url)
    req.header("Content-Type", "application/json")
    req.header("X-Webhook-Signature", "sha256=abc")   # real apps would compute HMAC
    req.data(payload)
    req.retries(3)
    req.timeout(5000)
    
    try {
        res = req.send()
        when res.isOk() {
            out "Webhook delivered: " + event
            give true
        }
        out "Webhook failed: HTTP " + str(res.statusCode)
        give false
    } catch e {
        out "Webhook error: " + e
        give false
    }
}

sendWebhook("https://hooks.slack.com/services/xxx", "user_signup", {"userId": "123", "email": "user@example.com"})
```

---

*Documentation generated from `E:\ezlib\ezhttp\main.ez` — EZ HTTP Client Library v2.0*
