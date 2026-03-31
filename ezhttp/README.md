# ezhttp — HTTP & JSON Library for EZ Language

A clean, simple HTTP client and JSON utility library for the EZ programming language.
Supports synchronous requests, async fetch, automatic JSON serialization, and URL encoding — all through two intuitive objects: `http` and `json`.

## Installation

```
ez install http
```

```ez
use "http"
```

---

## The `http` Object

### `http.get(url)`

Performs a synchronous HTTP GET request.

```ez
use "http"

response = http.get("https://api.example.com/users")
out response
```

### `http.getExt(url, headers)`

GET request with custom headers.

```ez
response = http.getExt("https://api.example.com/protected", {
    "Authorization": "Bearer my-token",
    "Accept": "application/json"
})
out response
```

### `http.post(url, body)`

Performs a synchronous HTTP POST request. If `body` is a dictionary or array, it is automatically serialized to JSON and the `Content-Type` header is set to `application/json`. If `body` is a plain string, it is sent as-is.

```ez
// Posting a dictionary — auto-serialized to JSON
response = http.post("https://api.example.com/users", {
    name: "Alice",
    email: "alice@example.com",
    age: 30
})
out response

// Posting a plain string
response = http.post("https://api.example.com/data", "hello world")
out response
```

### `http.postExt(url, body, headers)`

POST request with full control over headers. If `body` is a dictionary or array and `Content-Type` is not already in `headers`, it is set to `application/json` automatically.

```ez
response = http.postExt(
    "https://api.example.com/messages",
    { text: "Hello!", priority: "high" },
    {
        "Authorization": "Bearer my-token",
        "X-Custom-Header": "value"
    }
)
out response
```

### `http.fetch(url)`

Performs an **asynchronous** GET request. Returns a `Future` — use `await()` to retrieve the result. Useful for running multiple requests in parallel.

```ez
// Single async request
future = http.fetch("https://api.example.com/data")
result = await(future)
out result

// Parallel requests — all fire at the same time
f1 = http.fetch("https://api.example.com/users")
f2 = http.fetch("https://api.example.com/posts")
f3 = http.fetch("https://api.example.com/comments")

out await(f1)
out await(f2)
out await(f3)
```

### `http.fetchExt(url, options)`

Async request with full options — method, body, and headers.

```ez
future = http.fetchExt("https://api.example.com/login", {
    method: "POST",
    body: json.stringify({ username: "alice", password: "secret" }),
    headers: {
        "Content-Type": "application/json"
    }
})

response = await(future)
out response
```

Supported option fields:

| Field | Type | Description |
|-------|------|-------------|
| `method` | string | HTTP method — `"GET"`, `"POST"`, `"PUT"`, `"DELETE"`, etc. Defaults to `"GET"` |
| `body` | string | Request body (for POST/PUT) |
| `headers` | dictionary | Custom request headers |

### `http.urlEncode(str)`

URL-encodes a string, escaping special characters for safe use in query parameters.

```ez
query = http.urlEncode("hello world & more")
out query   // hello+world+%26+more

url = "https://api.example.com/search?q=" + http.urlEncode("EZ language")
response = http.get(url)
```

### `http.urlDecode(str)`

Decodes a URL-encoded string back to its original form.

```ez
decoded = http.urlDecode("hello+world+%26+more")
out decoded   // hello world & more
```

---

## The `json` Object

### `json.parse(string)`

Parses a JSON string and returns an EZ dictionary or array.

```ez
use "http"

text = '{"name": "Alice", "age": 30, "active": true}'
data = json.parse(text)

out data["name"]     // Alice
out str(data["age"]) // 30
```

### `json.stringify(obj)`

Converts an EZ dictionary or array into a JSON string.

```ez
user = {
    name: "Bob",
    scores: [95, 87, 92],
    active: true
}

text = json.stringify(user)
out text
// {"name":"Bob","scores":[95,87,92],"active":true}
```

---

## Complete Examples

### Fetching and Parsing a REST API

```ez
use "http"

raw = http.get("https://jsonplaceholder.typicode.com/posts/1")
post = json.parse(raw)

out "Title: " + post["title"]
out "Body:  " + post["body"]
```

### Submitting a Form

```ez
use "http"

payload = {
    username: "alice",
    password: "secret123"
}

response = http.post("https://api.example.com/login", payload)
result = json.parse(response)

when result["success"] == true {
    out "Logged in! Token: " + result["token"]
} other {
    out "Login failed: " + result["error"]
}
```

### Authenticated API Request

```ez
use "http"

token = "eyJhbGciOiJIUzI1NiJ9..."

response = http.getExt("https://api.example.com/profile", {
    "Authorization": "Bearer " + token,
    "Accept": "application/json"
})

profile = json.parse(response)
out "Welcome, " + profile["name"]
```

### Parallel Async Requests

```ez
use "http"

// Fire all three at the same time
f1 = http.fetch("https://jsonplaceholder.typicode.com/users/1")
f2 = http.fetch("https://jsonplaceholder.typicode.com/users/2")
f3 = http.fetch("https://jsonplaceholder.typicode.com/users/3")

// Collect results when needed
u1 = json.parse(await(f1))
u2 = json.parse(await(f2))
u3 = json.parse(await(f3))

out u1["name"]
out u2["name"]
out u3["name"]
```

### Building a Query String

```ez
use "http"

base = "https://api.example.com/search"
query = base + "?q=" + http.urlEncode("hello world") + "&page=1"

response = http.get(query)
out response
```

### Sending a PUT Request

```ez
use "http"

future = http.fetchExt("https://api.example.com/users/42", {
    method: "PUT",
    body: json.stringify({ name: "Alice Updated", age: 31 }),
    headers: { "Content-Type": "application/json" }
})

out await(future)
```

### Sending a DELETE Request

```ez
use "http"

future = http.fetchExt("https://api.example.com/users/42", {
    method: "DELETE",
    headers: { "Authorization": "Bearer my-token" }
})

out await(future)
```

---

## API Reference

### `http`

| Method | Signature | Returns | Description |
|--------|-----------|---------|-------------|
| `get` | `get(url)` | string | Synchronous GET request |
| `getExt` | `getExt(url, headers)` | string | GET request with custom headers |
| `post` | `post(url, body)` | string | Synchronous POST — auto JSON if dict/array |
| `postExt` | `postExt(url, body, headers)` | string | POST request with custom headers |
| `fetch` | `fetch(url)` | Future | Async GET — use `await()` to resolve |
| `fetchExt` | `fetchExt(url, options)` | Future | Async request with method, body, headers |
| `urlEncode` | `urlEncode(str)` | string | URL-encode a string |
| `urlDecode` | `urlDecode(str)` | string | URL-decode a string |

### `json`

| Method | Signature | Returns | Description |
|--------|-----------|---------|-------------|
| `parse` | `parse(string)` | dict / array | Parse a JSON string into an EZ value |
| `stringify` | `stringify(obj)` | string | Convert an EZ dict/array to a JSON string |

---

## Notes

- `http.post()` and `http.postExt()` automatically detect dictionaries and arrays and serialize them to JSON — you rarely need to call `json.stringify()` manually before posting.
- `http.fetch()` and `http.fetchExt()` are non-blocking. They return a `Future` immediately. Fire multiple fetches before awaiting any of them for true parallel execution.
- All synchronous methods (`get`, `post`) block until the server responds.
- Responses are always returned as raw strings. Use `json.parse()` to work with JSON API responses as EZ dictionaries.

---

## License

MIT License — see the [LICENSE](LICENSE) file for details.
