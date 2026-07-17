# ezauth — Authentication for the EZ web framework

> **Version:** 1.0.0  
> **Import:** `use "auth"`  
> **File:** `C:\ezlib\auth\main.ez`  
> **Requires:** [`web`](../web) ≥ 2.0.2, and the bundled `dlls/auth_accel.dll`

---

## Overview

`ezauth` does what `flask-login` does — track who is logged in and gate routes on it —
plus the two things `flask-login` deliberately leaves to other packages but which no real
app can skip: **password hashing** and **CSRF**. A login library that punts on hashing
invites `md5(password)`, so it is included here.

- **Password hashing** — PBKDF2-HMAC-SHA256, salted, constant-time verification
- **Session login** — `login_user` / `logout_user` / `current_user`
- **Route guards** — `login_required`, `roles_required`
- **CSRF** — per-session tokens and a `csrf_protect()` middleware
- **Session fixation defence** — the session id is rotated on every login and logout

Everything that must be slow-by-design, constant-time, or random comes from
`auth_accel.dll`, which calls Windows CNG (`bcrypt.dll`). None of it is reimplemented in
EZ. The hashes are byte-for-byte identical to Python's
`hashlib.pbkdf2_hmac("sha256", ...)`, so they are portable to and from Django-style
systems.

---

## Quick Start

```ez
use "web"
use "auth"

app = WebApp()
auth = LoginManager(app)

users = {
    "ana": {"id": 1, "name": "ana", "roles": ["admin"],
            "pw": hash_password("hunter2")}
}
by_id = {"1": users["ana"]}

# Required: teach ezauth how to turn a stored id back into a user.
auth.user_loader(|uid| {
    when has_key(by_id, str(uid)) { give by_id[str(uid)] }
    give nil
})

auth.login_view = "/login"          # anonymous users get redirected here

app.post("/login", |req| {
    u = users[req["form"]["username"]]
    when typeOf(u) == "nil" { give {"status": 401, "body": "bad credentials"} }
    when not verify_password(req["form"]["password"], u["pw"]) {
        give {"status": 401, "body": "bad credentials"}
    }
    auth.login_user(req, u)
    give redirect("/dash")
})

app.post("/logout", |req| { auth.logout_user(req)  give redirect("/") })

app.get("/dash", auth.login_required(|req| {
    give "Hello, " + auth.current_user(req)["name"]
}))

app.get("/admin", auth.roles_required(["admin"], |req| { give "admin area" }))

app.run(8080)
```

Always answer a failed login with the **same** message whether the username was unknown
or the password was wrong. "No such user" versus "wrong password" tells an attacker which
accounts exist.

---

## Password Hashing

### `hash_password(password, iterations = PBKDF2_ITERATIONS)` → `string`

Returns `pbkdf2_sha256$<iterations>$<salt>$<hash>`. Store the whole string — it carries
its own salt and cost, so the cost can be raised later without invalidating old hashes.

```ez
u["pw"] = hash_password("hunter2")
```

The salt is random per call, so **hashing the same password twice gives different
output**. That is correct: it means identical passwords are not visible as identical
hashes in your user table, and cracking one does not crack the rest.

### `verify_password(password, encoded)` → `bool`

Constant-time. Returns `false` — never throws — for a nil, empty, truncated, or
otherwise malformed stored hash, so a corrupt row can never become a successful login.

### `needs_rehash(encoded)` → `bool`

True if a stored hash was made with fewer iterations than `PBKDF2_ITERATIONS`. A user's
successful login is the only moment you hold their plaintext, so it is the only moment
you can upgrade:

```ez
when verify_password(pw, u["pw"]) {
    when needs_rehash(u["pw"]) { u["pw"] = hash_password(pw)  save(u) }
    auth.login_user(req, u)
}
```

### `PBKDF2_ITERATIONS`

Defaults to **600,000** — OWASP's 2023 recommendation for PBKDF2-HMAC-SHA256, measured
at **~160 ms** per call here. That slowness is the entire point: it costs an attacker the
same, once per guess.

Two consequences worth planning for:

- **A login blocks other requests for its duration.** Handlers are serialized within a
  process, so ~160 ms of hashing is ~160 ms nobody else is served. Run
  `app.run(port=..., workers=N)` to hash on N processes at once.
- **Rate-limit your login route regardless** (`app.useRateLimiter(...)`). An expensive
  login endpoint is otherwise a denial-of-service lever.

Lowering this trades your users' passwords for latency. Do it deliberately, and only in
tests:

```ez
h = hash_password("pw", 1000)   # fast, for tests ONLY
```

---

## Login Manager

### `LoginManager(app = nil)`

Passing `app` also sets `app.auth`, so the manager can be reached from anywhere you have
the app.

### `auth.user_loader(fn)` — **required**

`fn` receives the stored id and returns the user, or `nil` if there is no such user.

```ez
auth.user_loader(|uid| { give db_find_user(uid) })
```

Returning `nil` makes the session count as logged out — which is what makes **deleting a
user take effect immediately**, on their next request, without hunting down their
sessions.

### `auth.login_user(req, user, id_field = "id")`

Marks the session as logged in. `user` may be a dictionary carrying an id, or the id
itself. Only the **id** is stored in the session: putting the whole user there would
serialize password hashes into your session store and go stale the moment the record
changes.

This also **rotates the session id** — see [Session Fixation](#session-fixation).

### `auth.logout_user(req)`

Clears the login and the CSRF token, and rotates the session id again so the previously
authenticated id is retired rather than left valid.

### `auth.current_user(req)` → user or `nil`

Calls `user_loader` at most once per id per request.

### `current_user_id(req)` → id or `nil`
### `is_authenticated(req)` → `bool`

Both read the session directly and never call `user_loader`. Use these when you only need
to know *whether* someone is logged in.

---

## Route Guards

### `auth.login_required(handler)` → handler

```ez
app.get("/dash", auth.login_required(|req| { give "members only" }))
```

Anonymous users — and users whose session names an id `user_loader` can no longer
resolve — get `auth.login_view` (as a redirect carrying `?next=`) if set, otherwise
`401`.

### `auth.roles_required(roles, handler, role_field = "roles")` → handler

```ez
app.get("/admin", auth.roles_required(["admin"], |req| { give "admin" }))
```

Passes if the user holds **any** of `roles`. `role_field` may be an array or a single
string. Anonymous → `401`; logged in but unauthorized → `403`. The distinction matters:
`401` means "log in", `403` means "logging in as someone else will not help".

### `auth.login_view`
### `auth.unauthorized_handler(fn)` / `auth.forbidden_handler(fn)`

Custom responses for the 401 and 403 cases.

The `?next=` value is always a path taken from the current request and is URL-encoded. It
is never a caller-supplied absolute URL — that is how open-redirect bugs get built. If
you consume `next` yourself, reject anything that does not start with a single `/`.

---

## CSRF

Browsers attach cookies to cross-site requests too, so without a token any other site can
POST to yours using your user's session. `SameSite=Lax` (which the web framework sets)
blocks the common case, but it is a browser-side default rather than a guarantee — a
per-session token the attacker cannot read is the actual defence.

### `csrf_token(req)` → `string`

Mints one on first use and keeps it for the session.

```ez
app.get("/settings", auth.login_required(|req| {
    give render_template("settings.html", {"csrf": csrf_token(req)})
}))
```

```html
<form method="post">
  <input type="hidden" name="_csrf" value="{{ csrf }}">
</form>
```

### `csrf_valid(req)` → `bool`

Checks the `_csrf` form field, then the `X-CSRF-Token` header (for `fetch`/XHR).
**Fails closed**: no token in the session means nothing to verify against, so it returns
`false`.

### `csrf_protect()` → middleware

```ez
app.use(csrf_protect())
```

Rejects any non-`GET`/`HEAD`/`OPTIONS` request without a valid token with `403`. Those
three pass through because they are supposed to be side-effect free, and gating them
would break ordinary navigation — which also means **do not perform state changes in a
`GET` handler**, or you have opted out of this protection.

Logging in issues a fresh CSRF token, since the pre-login one belongs to the pre-login
session.

---

## Session Fixation

`login_user` and `logout_user` call `regenerate_session(req)`, which retires the current
session id server-side and issues a new one, preserving the session contents.

This closes a real attack. Without rotation, an attacker who can plant a known session id
in the victim's browser — via a link, a subdomain, or an XSS-set cookie — simply waits
for the victim to log in. The id the attacker holds is now an authenticated session. With
rotation, the id they planted is retired at the moment of login and the id that carries
the privileges is one they never saw.

Verified end to end: after login the cookie changes, and the pre-login id is dead — it
resolves to anonymous and `login_required` routes answer `401`.

You should call `regenerate_session(req)` yourself on any other privilege change, such as
a password change or a step-up to admin.

---

## Utilities

### `sign(key, message)` → hex `string`

HMAC-SHA256. For signing values that travel through the client, so tampering is
detectable without server-side state.

### `secure_equals(a, b)` → `bool`

Constant-time string comparison. Use it for tokens and digests: `==` returns at the first
differing byte, so its timing leaks how long a prefix was correct — enough to reconstruct
a secret one byte at a time.

---

## Notes & Limits

- **Sessions are the web framework's.** With `workers > 1`, the default in-memory session
  store gives each worker its own table and users appear randomly logged out. Use a
  shared store: `app.setSessionStore(FileSessionStore("./sessions"))`.
- **Serve auth over HTTPS.** A session cookie is a bearer token; on plain HTTP anyone on
  the path can read and replay it. The framework sets `HttpOnly; SameSite=Lax`, but
  `Secure` needs a real TLS deployment (typically a reverse proxy in front).
- **No remember-me cookie yet.** Sessions last as long as the session cookie.
- **`user_loader` runs on every guarded request.** Keep it cheap, or cache it.
- **Windows only**, like the rest of the stack: `auth_accel.dll` uses Windows CNG.

---

## Building the DLL

```
cd C:\ezlib\auth\src
build.bat
```

Requires MSYS2/MinGW at `C:\msys64\mingw64`. `mingw64\bin` must be on `PATH` — invoking
`g++` by absolute path alone leaves `cc1plus.exe` unable to find its own dependencies,
and it fails with exit code 1 and no message at all.

---

## Tests

```
cd C:\ezlib\auth
ez test_auth.ez
```

61 assertions covering hashing, salting, malformed-hash rejection, rehash detection, HMAC
(against a known reference vector), constant-time comparison, login/logout, the route
guards, and CSRF.
