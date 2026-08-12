# ezlib — the EZ standard library

Every package published to [packages.ez-lang.site](https://packages.ez-lang.site),
at the version currently released. **69 packages.**

This repository is a snapshot of the registry, not the source of truth for it.
The registry is what `ez install` talks to; this is here to be read, diffed and
browsed.

## Installing

```
ez install postgres
ez install httpx text cron
```

Packages install into the shared library root — `C:\ezlib` by default, or
wherever `EZLIB_PATH` points — so the compiler resolves `use "postgres"` from
there without any per-project setup. Dependencies come down with the package,
and `ez.lock` records the exact versions.

```ez
use "postgres"
use "httpx"
```

To reproduce this whole tree:

```
ez install <every name below>
```

## What's here

### Web and HTTP

| package | ver | |
|---|---|---|
| **web** | 1.0.0 | Flask/Express style web framework |
| **serve** | 1.0.0 | Flask/Express style web framework |
| **httpx** | 1.0.0 | HTTP client over WinHTTP — real status codes and response headers, any verb, byte-exact bodies, JSON and form helpers, retries, downloads |
| **http** | 1.0.0 | HTTP client with Request/Response models, retries, timeouts, interceptors |
| **websocket** | 1.0.0 | RFC 6455 client over pure TCP — handshake, masking, fragmentation, ping/pong, close |
| **socket** | 1.0.0 | TCP and UDP over pure FFI — hostname resolution, timeouts, binary-safe transfers |
| **auth** | 1.0.0 | Session authentication, password hashing and CSRF for the web framework |
| **oauth** | 1.0.0 | OAuth 2.0 client — authorization code flow with PKCE, refresh, client credentials, and presets for Google, GitHub, Microsoft, Discord, Slack, GitLab, Spotify, LinkedIn, Facebook, Twitch |
| **jwt** | 1.0.0 | JSON Web Tokens, HS256 |
| **ratelimit** | 1.0.0 | Request throttling — sliding window and token bucket, in-memory or Redis-backed, with the conventional headers |
| **template** | 1.0.0 | Templating with variables, filters, conditionals and loops, HTML autoescaping by default |

### Databases and storage

| package | ver | |
|---|---|---|
| **postgres** | 1.0.0 | PostgreSQL over libpq — parameterised queries, pooling, transactions with savepoints and retry, full type mapping, SQLSTATE-aware errors |
| **sqlite** | 1.0.0 | SQLite over pure FFI — prepared statements, transactions, migrations |
| **db** | 1.0.0 | SQLite framework with CRUD helpers, query builders and transactions |
| **orm** | 1.1.0 | SQLAlchemy-style ORM — models, relationships, identity map, sessions |
| **migrate** | 1.0.0 | Schema migrations — ordered up/down steps, a ledger, transactional application, drift detection |
| **redis** | 1.0.0 | Redis over pure TCP — RESP, strings, hashes, lists, sets, expiry, URL connection strings |
| **s3** | 1.0.0 | S3-compatible object storage — Signature V4, presigned URLs, presets for R2, Spaces and MinIO |
| **cache** | 1.0.0 | In-memory caching — TTL expiry, LRU eviction, compute-on-miss, memoisation, hit-rate stats |
| **queue** | 1.0.0 | Background job queues — in-memory and Redis drivers, delayed jobs, retries with backoff, stalled-job recovery |

### Data formats

| package | ver | |
|---|---|---|
| **json** | 1.0.0 | JSON parser and stringifier |
| **yaml** | 1.0.0 | YAML — mappings, sequences, flow collections, block scalars, round-trip stringify |
| **toml** | 1.0.0 | TOML — nested tables, arrays of tables, inline tables, round-trip serialisation |
| **ini** | 1.0.0 | INI — sections, typed values, comments, quoting, round-trip serialisation |
| **xml** | 1.0.0 | XML parser and builder — attributes, CDATA, comments, entities, find/findAll |
| **csv** | 1.0.0 | CSV parser and stringifier, RFC 4180 |
| **markdown** | 1.0.0 | Markdown to HTML with escape-first rendering that cannot emit raw HTML |
| **diff** | 1.0.0 | Text and sequence diffing — LCS line/word/character diffs, unified patches, reversible edit scripts |

### Security

| package | ver | |
|---|---|---|
| **crypto** | 1.0.1 | AES-256, SHA hashing, HMAC, PBKDF2, Base64 |
| **password** | 1.0.0 | PBKDF2-HMAC-SHA256 with per-hash salts and parameters, timing-safe verification, rehash detection |
| **totp** | 1.0.0 | Time-based one-time passwords (RFC 6238/4226) — 2FA codes, base32, otpauth URIs |

### Text, numbers and validation

| package | ver | |
|---|---|---|
| **text** | 1.0.0 | String utilities that count characters, not bytes — slugs, case conversion, wrapping, truncation, pluralisation, masking, fuzzy comparison |
| **regex** | 1.0.0 | Compiled patterns, match positions, capture groups, flags, split/replace, validators |
| **decimal** | 1.0.0 | Exact decimal and money arithmetic — fixed-point, no float drift, rounding modes, remainder-safe splitting |
| **math** | 1.0.0 | Numeric helpers, trigonometry, statistics, number theory, random sampling, geometry, vectors |
| **validate** | 1.0.0 | Schema validation — chainable rules, coercion, every error reported at once |
| **semver** | 1.0.0 | Semantic versions — parsing, comparison, caret/tilde/wildcard/interval ranges |
| **ulid** | 1.0.0 | Sortable identifiers — 48-bit timestamp plus 80 bits of randomness, monotonic within a millisecond |
| **i18n** | 1.0.0 | Translation and localisation — nested catalogues, interpolation, CLDR plural rules, locale-aware number and currency formatting |
| **collections** | 1.0.0 | Set, Map, Queue, Stack, LinkedList, Deque, Counter |

### Time and scheduling

| package | ver | |
|---|---|---|
| **datetime** | 1.1.0 | Timestamps, components, formatting and arithmetic, plus real time zones with DST, ISO 8601 parsing, calendar maths and relative time |
| **cron** | 1.0.0 | Cron expressions, next-occurrence calculation, and an in-process scheduler with failure handling |
| **calendar** | 1.0.0 | Full month and year calendars |
| **retry** | 1.0.0 | Retries, exponential backoff with jitter, and circuit breaking |
| **taskschd** | 1.0.0 | Windows Task Scheduler |
| **thread** | 1.0.0 | Multithreading and task management |

### Command line

| package | ver | |
|---|---|---|
| **args** | 1.0.0 | Argument parsing — flags, options, positionals, type coercion, choices, generated help |
| **color** | 1.0.0 | ANSI colour and styling — 256-colour and truecolor, chainable styles, hex conversion, ANSI-aware width |
| **table** | 1.0.0 | Terminal tables — auto-sized columns, alignment, truncation, several border styles including markdown |
| **progress** | 1.0.0 | Progress bars and spinners — in-place animation, percentages, ETA, rates, byte formatting |
| **testing** | 1.0.0 | Unit testing — assertions, suites, hooks, mocks, coloured reporting |
| **log** | 1.0.0 | Logging with terminal colours and file sinks |

### System

| package | ver | |
|---|---|---|
| **os** | 1.0.0 | Env, paths, processes, shell, clipboard, system info, known directories |
| **fs** | 1.0.0 | File handling |
| **env** | 1.0.0 | `.env` files — parsing, validation, required-key checks, type coercion |
| **ffi** | 1.0.0 | Native FFI, ctypes-style — type annotations, named structs, callbacks, unlimited arity |
| **clipboard** | 1.0.0 | System clipboard |
| **notify** | 1.0.0 | Desktop notifications and dialog prompts |
| **auto** | 1.0.0 | Desktop automation — keyboard and mouse control |

### Media and devices

| package | ver | |
|---|---|---|
| **gui** | 1.0.0 | Native Win32 GUI |
| **game** | 1.0.0 | GUI game engine |
| **audio** | 1.0.0 | mp3 and wav playback |
| **webcam** | 1.0.0 | Live webcam stream and photo capture |
| **qr** | 1.0.0 | QR code generation via libqrencode |
| **pdf** | 1.0.0 | PDF generation |

### Messaging and services

| package | ver | |
|---|---|---|
| **email** | 1.0.0 | SMTP — plain, HTML, attachments, CC/BCC, multiple recipients |
| **mailer** | 1.0.0 | High-level mailer — HTML templates, connection pooling, presets for Gmail, SendGrid, Mailgun, AWS SES, Outlook |
| **whatsapp** | 1.0.0 | WhatsApp Web client — Noise Protocol over WebSocket |
| **ai** | 1.0.0 | OpenAI, Anthropic and Gemini in one call |

## Dependencies

Most packages stand alone. Those that don't:

```
auth      → web            oauth      → crypto        s3         → httpx, crypto
jwt       → crypto         os         → fs            serve      → fs
log       → fs             password   → crypto        table      → color
mailer    → email          progress   → color         taskschd   → os, csv
migrate   → orm            queue      → redis         totp       → crypto
                           redis      → socket        websocket  → socket, crypto
```

`ez install` resolves these for you.

## Platform

EZ runs on Windows, and a good number of these packages bind directly to
Windows APIs through the FFI rather than shipping a native module:

- `crypto` and `password` — CryptoAPI and BCrypt
- `httpx` — WinHTTP
- `datetime` — the Windows time-zone database, for DST
- `gui`, `game`, `audio`, `webcam`, `clipboard`, `notify`, `auto`, `os`,
  `taskschd` — Win32
- `socket`, `sqlite`, `ffi` — FFI with no C++ in the interpreter

Three need a library that does not ship with Windows:

- `postgres` — `libpq.dll`, from a PostgreSQL install (the client alone is
  enough). It searches `PATH`, the usual install directories, and a local
  `dlls/` folder.
- `qr` — `libqrencode`
- `email` — `libcurl`

## Tests

34 of the 69 packages ship a `test.ez` you can run directly:

```
cd postgres
ez test.ez
```

Those suites run against real dependencies rather than mocks where it matters —
`postgres` against a live server, `httpx` and `s3` against real HTTPS
endpoints, `redis` against a real Redis. They skip with a reported count when
the dependency is not there rather than passing quietly.

## Contributing

Packages are published to the registry, not merged here:

```
cd mypackage
ez publish
```

A package needs a `package.ez` manifest with `name`, `version`, `main` and
`description`. Published versions are immutable — to change something, publish
a new version.

Two things worth knowing before writing EZ library code, both of which cause
silent breakage rather than an error:

- Assigning to a **builtin** name (`num = 0`) replaces it for the whole
  process, so every later `num(...)` fails.
- Assigning to a name that matches a **task declared earlier in the same file**
  does the same to that task. A task declared *below* the assignment is safe.

## License

Each package carries its own licence; all are MIT.
