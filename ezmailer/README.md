# mailer

**High-level mailer for EZ — provider presets, HTML templates, and connection pooling on top of `email`.**

`mailer` is a convenience layer built on top of the [`email`](#) SMTP package. It doesn't talk to `libcurl` or touch FFI at all — instead it wraps `email.connect()` / `email.Message` / `email.SmtpClient` with three things application code actually wants: one-liner provider setup (Gmail, SendGrid, Mailgun, SES, Outlook, Yahoo), a small `{{ key }}` HTML template engine for transactional email, and a connection pool for bulk sending.

```
depends on: email (email)
```

---

## Table of Contents

- [Relationship to `email` / `email`](#relationship-to-email--email)
- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Usage Guide](#usage-guide)
  - [Provider Presets](#provider-presets)
  - [Templates](#templates)
  - [`sendTemplate` Helper](#sendtemplate-helper)
  - [Connection Pool](#connection-pool)
- [Error Handling](#error-handling)
- [Architecture](#architecture)
- [File-by-File Reference](#file-by-file-reference)
- [Thread Safety](#thread-safety)
- [Design Notes & Caveats](#design-notes--caveats)
- [License](#license)

---

## Relationship to `email` / `email`

`mailer` is **not** a replacement for `email` — it's built directly on top of it:

- `Providers.*` and `createPool()` both return/wrap `SmtpClient` instances created via `email.connect()`.
- `Message` (used when building emails to send) is the same `Message` model exported by `email`.
- All the low-level SMTP mechanics — curl lifecycle, MIME payload building, TLS config, temp-file handling — still live in `email`. `mailer` never touches curl or FFI directly.

If you only need to send a single plain email, `email` alone is enough. Reach for `mailer` when you want provider shortcuts, file-based HTML templates, or pooled bulk sending.

---

## Features

- **Provider presets** — one function call each for Gmail, Gmail SSL, Outlook, Yahoo, SendGrid, Mailgun, Amazon SES, or a custom SMTP host.
- **HTML templates** — author transactional emails as `.html` files with `{{ key }}` placeholders instead of giant inline strings.
- **`sendTemplate()`** — render a template and send it in one call.
- **Connection pooling** — pre-allocate N `SmtpClient`s and reuse them across a bulk send instead of paying curl init cost per message.
- **Batch sending** — `pool.sendAll(messages)` sends a whole list, collecting failures instead of aborting on the first one.
- **Structured exceptions** — `MailerError`, `PoolExhaustedError`, `TemplateError`, layered on top of `email`'s own exception hierarchy.

---

## Installation

```ez
use "mailer"
```

This pulls in `main.ez`, which internally imports `email` (the `email` package) plus `src/errors.ez`, `src/template.ez`, `src/pool.ez`, and `src/providers.ez`. As with `email`, you only ever need the single top-level `use` — the `src/` files are implementation details.

> **Prerequisite:** `mailer` requires `email` to be installed/importable as `"email"`, since it calls `email.connect()` under the hood and reuses `email.Message`.

---

## Quick Start

### Provider preset + template + send

```ez
use "mailer"

client = Providers.gmail("me@gmail.com", "app-pass")
html   = Template.render("emails/welcome.html", {"name": "Alice"})

client.send(
    Message()
        .from("me@gmail.com")
        .to("alice@example.com")
        .subject("Welcome!")
        .html(html)
        .attach("welcome_guide.pdf")
)
```

### Connection pool for bulk sending

```ez
use "mailer"

pool = createPool("smtp.gmail.com", 587, "me@gmail.com", "pass", 3)
pool.sendAll(messages)
```

---

## Usage Guide

### Provider Presets

`Providers` is a static model — call its tasks directly without instantiating it. Every preset returns a ready-to-use `SmtpClient` (the same type `email.connect()` returns), so `.send()` works immediately.

```ez
client = Providers.gmail("me@gmail.com", "app-password")
client = Providers.gmailSSL("me@gmail.com", "app-password")   // port 465 / SMTPS
client = Providers.outlook("me@outlook.com", "password")
client = Providers.yahoo("me@yahoo.com", "app-password")
client = Providers.sendgrid("SG.xxxxxxxxxx")
client = Providers.mailgun("mg.myapp.com", "smtp-password")
client = Providers.ses("AKID...", "smtp-secret...", "us-east-1")
client = Providers.custom("mail.myserver.com", 587, "user", "pass")
```

| Preset | Host | Port | Username | Notes |
|---|---|---|---|---|
| `gmail(username, appPassword)` | `smtp.gmail.com` | 587 | your Gmail address | Requires a **Google App Password**, not your regular password (Google Account → Security → 2-Step Verification → App Passwords) |
| `gmailSSL(username, appPassword)` | `smtp.gmail.com` | 465 | your Gmail address | Implicit SSL/SMTPS variant of the above |
| `outlook(username, password)` | `smtp-mail.outlook.com` | 587 | your Outlook/Hotmail address | — |
| `yahoo(username, appPassword)` | `smtp.mail.yahoo.com` | 587 | your Yahoo address | Requires a Yahoo App Password |
| `sendgrid(apiKey)` | `smtp.sendgrid.net` | 587 | literal string `"apikey"` | `apiKey` is your SendGrid key, starting with `SG.` |
| `mailgun(domain, smtpPassword)` | `smtp.mailgun.org` | 587 | `postmaster@<domain>` | `smtpPassword` is the Mailgun **SMTP password**, not the general API key |
| `ses(smtpUser, smtpPassword, region)` | `email-smtp.<region>.amazonaws.com` | 587 | SES SMTP username | Credentials come from the SES console's SMTP Settings, not your AWS access key |
| `custom(server, port, username, password)` | any | any | any | Escape hatch for anything not covered above |

All presets default to port 587 with STARTTLS except `gmailSSL` (465/implicit SSL). TLS behavior beyond that (cert verification, min TLS version, CA bundle, verbose logging) is still controlled through the returned `SmtpClient`'s fields, exactly as in `email`:

```ez
client = Providers.custom("mail.internal.corp", 25, "svc", "pass")
client.verifyCert = false   // ⚠ internal/self-signed servers only
```

### Templates

`Template` is a static model for rendering `{{ key }}`-style HTML templates.

```ez
html = Template.render("emails/welcome.html", {
    "name":    "Alice",
    "confirm": "https://myapp.com/confirm/abc123"
})
```

**Syntax rules:**

- Placeholders look like `{{ key }}` or `{{key}}` — the space inside the braces is optional; both forms are replaced.
- Keys are **case-sensitive**.
- Unknown keys (present in the template but missing from `vars`) are left in the output as-is — this is not an error.
- Values are stringified with `str()` before substitution.
- **HTML is not escaped.** If you're injecting user-supplied data into a template, sanitize it yourself before calling `render()`/`inject()`.

Three entry points:

| Task | Purpose |
|---|---|
| `Template.render(templatePath, vars)` | Reads an HTML file from disk and injects `vars`. Throws `TemplateError` if the file is missing, unreadable, or empty. |
| `Template.renderString(html, vars)` | Same substitution logic, but for an HTML string already in memory (no file I/O). |
| `Template.inject(html, vars)` | The underlying substitution routine both of the above call. |

Example template file (`emails/welcome.html`):

```html
<h1>Welcome, {{ name }}!</h1>
<p>Click <a href="{{ confirm_url }}">here</a> to confirm your account.</p>
```

### `sendTemplate` Helper

For the common case of "render a template, attach it as the HTML body, send it," `main.ez` exposes a one-call helper:

```ez
sendTemplate(client, msg, templatePath, vars)
```

```ez
mailer.sendTemplate(
    Providers.gmail("me@gmail.com", "app-pass"),
    Message().from("me@gmail.com").to("alice@example.com").subject("Welcome!"),
    "emails/welcome.html",
    {"name": "Alice", "confirm_url": "https://app.com/confirm/abc"}
)
```

Internally this is just:

```ez
html = Template.render(templatePath, vars)
msg.html(html)
give client.send(msg)
```

— i.e. it mutates the passed-in `msg` by setting its HTML body, then sends it through the passed-in `client`. Build the `Message` with `from`/`to`/`subject` (and anything else) first; `sendTemplate` fills in the body.

### Connection Pool

`createPool()` (or `ConnectionPool` directly) pre-allocates a fixed number of `SmtpClient` instances so that repeated sends in a loop don't pay curl-init cost per message.

```ez
pool = createPool("smtp.gmail.com", 587, "me@gmail.com", "pass", maxSize = 5)
```

| Argument | Description |
|---|---|
| `server`, `port`, `username`, `password` | Same as `email.connect()` |
| `maxSize` | Number of `SmtpClient`s to pre-allocate (default `5`) |

**What the pool does and does not do:** it does *not* pre-open TCP sockets — libcurl manages actual connection reuse internally (`CURLOPT_MAXCONNECTS`). What it does manage is object-level checkout/checkin: it keeps N `SmtpClient` objects allocated, marks a slot "in use" while a send is in flight, and returns it to the pool when the send finishes (whether it succeeded or threw).

**Single send, borrowing from the pool:**

```ez
pool.send(msg)
```

- Checks out the first free client (throws `PoolExhaustedError` if all `maxSize` slots are busy).
- Sends the message.
- Always checks the client back in, even if `send()` throws.
- Re-throws the original send error (if any) after checking in.

**Batch send:**

```ez
sent = pool.sendAll(messages)
```

- Iterates the list, calling `pool.send()` for each message.
- A failure on one message does **not** stop the batch — every message is attempted.
- Returns the count of successful sends.
- If one or more messages failed, throws a single `MailerError` summarizing the failure count *after* the whole batch has been attempted. Individual per-message exceptions are swallowed into that count, not exposed individually — wrap `pool.send()` yourself in a loop if you need per-message error detail.

**Inspecting pool state:**

```ez
pool.status()
// { "total": 5, "in_use": 2, "available": 3 }
```

---

## Error Handling

`mailer` adds three exception types on top of everything `email` already throws (`EmailError`, `SmtpConnectionError`, `SmtpAuthError`, `AttachmentError` — see the `email` README):

| Exception | Thrown When |
|---|---|
| `MailerError` | Generic mailer-level failure; also used by `pool.sendAll()` to summarize batch failures |
| `PoolExhaustedError` | `pool.send()`/`pool.sendAll()` is called when all `maxSize` connections are already checked out |
| `TemplateError` | A template file is missing, unreadable, or empty |

All three implement `toString()`:

```ez
try {
    pool.send(msg)
} catch (e) {
    out e.toString()
    // e.g. "PoolExhaustedError: All 5 SMTP connections are in use. Try again later or increase pool size."
}
```

Because `mailer` builds directly on `email`, a `pool.send()` or `client.send()` call can also raise any of `email`'s own exceptions (`SmtpAuthError`, `SmtpConnectionError`, `AttachmentError`, `EmailError`) — catch broadly if you want to handle both layers uniformly.

---

## Architecture

```
                    ┌───────────────┐
                    │   main.ez     │   createPool() / sendTemplate()
                    └───────┬───────┘
                            │ use
         ┌──────────────────┼──────────────────┬───────────────┐
         ▼                  ▼                  ▼               ▼
  src/providers.ez     src/pool.ez       src/template.ez  src/errors.ez
         │                  │                    │               ▲
         │                  │                    └────use────────┤
         │                  └──────────use───────────────────────┤
         └─────────────────────────use───────────────────────────┘
                            │
                            ▼
                     "email" (email)
                  connect() / Message / SmtpClient
```

- **`src/providers.ez`** and **`src/pool.ez`** both call `email.connect()` — neither one duplicates SMTP/curl logic.
- **`src/template.ez`** is pure string manipulation and file I/O — no SMTP awareness at all.
- **`src/errors.ez`** defines mailer-specific exceptions, separate from (but coexisting with) `email`'s exception hierarchy.
- **`main.ez`** is the only file application code needs to `use` — it re-exports `Providers`, `Template`, `ConnectionPool` (via `createPool`), and the `sendTemplate` convenience task.

---

## File-by-File Reference

| File | Responsibility |
|---|---|
| `main.ez` | Public API: `createPool()`, `sendTemplate()`. Imports `email` plus all `src/` modules. |
| `src/errors.ez` | Exception models: `MailerError`, `PoolExhaustedError`, `TemplateError`. |
| `src/template.ez` | `Template` static model — `render()` (from file), `renderString()` (from memory), `inject()` (substitution engine) for `{{ key }}` placeholders. |
| `src/pool.ez` | `ConnectionPool` model — pre-allocates `SmtpClient`s, checkout/checkin, `send()`, `sendAll()`, `status()`. |
| `src/providers.ez` | `Providers` static model — one-liner presets for Gmail, Gmail SSL, Outlook, Yahoo, SendGrid, Mailgun, SES, and custom SMTP. |

---

## Thread Safety

`ConnectionPool` instances are **not** safe to share across threads. Per the source comments: EZ's `spawn()` model gives each thread its own VM, so a pool (and the `SmtpClient` objects inside it) created on one thread is not visible to or usable by another. **Create one pool per thread**, not one shared pool.

---

## Design Notes & Caveats

- **No HTML escaping.** `Template.inject()` performs raw string substitution with no escaping. Sanitize any user-controlled values before passing them into `vars`, or you risk HTML/markup injection into outgoing emails.
- **Unknown template keys are silent.** If your template references `{{ typo }}` and `vars` doesn't have that key, the literal `{{ typo }}` text is left in the rendered output rather than raising an error — worth double-checking rendered output during development.
- **`sendTemplate` mutates its `msg` argument.** It calls `msg.html(html)` on the `Message` you pass in rather than returning a new one.
- **Batch errors are aggregated, not itemized.** `pool.sendAll()` reports only a failure *count* via `MailerError`; it doesn't return which specific messages failed or why. For per-message error handling, loop over your message list and call `pool.send()` yourself.
- **Pool sizing is static.** `maxSize` is fixed at construction time — there's no dynamic growth/shrink; a `PoolExhaustedError` under sustained load means you need a bigger pool (or a queue in front of it), not that the pool will scale itself.
- **Depends on `email`.** All the caveats in the `email` README apply transitively — Windows-oriented FFI/DLL loading, TLS defaults, temp-file-based payload delivery, etc.
