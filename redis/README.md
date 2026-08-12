# redis — Redis client for EZ

> **Import:** `use "redis"`
> **Install:** `ez install redis`
> **Depends on:** `socket`

Speaks RESP over a plain TCP socket. No native library, no DLL — it works
anywhere `socket` does.

```ez
use "redis"

r = Redis("127.0.0.1", 6379)
r.set("greeting", "hello")
out r.get("greeting")
r.close()
```

---

## Why you want this

`cache` is in-memory and single-process: a restart loses everything, and two
`web` workers cannot see each other's entries. Redis gives you sessions that
survive a deploy, a cache shared across processes, and a queue that outlives
the program that filled it.

## Quick Start

```ez
use "redis"

r = Redis("127.0.0.1", 6379)

# Strings
r.set("user:1:name", "Ali")
r.setEx("session:abc", token, 3600)      # expires in an hour
out r.get("user:1:name")

# Counters
r.increment("page:views")

# Hashes
r.hashSet("user:1", "email", "a@b.co")
profile = r.hashAll("user:1")            # a dictionary

# Lists
r.listPush("jobs", "send-email")
job = r.listPopFront("jobs")

r.close()
```

## Connecting

```ez
Redis("127.0.0.1", 6379)
redisConnect("redis://localhost:6379")
redisConnect("redis://:password@host:6379/2")
redisConnect("redis://user:password@host:6379")
```

Connections open **lazily** on first use, so building a client is cheap and an
unreachable server surfaces at the call that needed it rather than at startup.

| Call | Effect |
| --- | --- |
| `r.timeout(ms)` | Socket timeout, default 5000. Returns the client |
| `r.connect()` | Force the connection open now |
| `r.close()` | Close it |
| `r.isAlive()` | `true` when the server answers — never throws |

## Commands

### Strings and keys

| Call | Redis |
| --- | --- |
| `r.set(k, v)` / `r.get(k)` | SET / GET |
| `r.getOr(k, fallback)` | GET with a default for a missing key |
| `r.setEx(k, v, seconds)` | SET with expiry |
| `r.setNx(k, v)` | SET if absent; `true` when it was set |
| `r.delete(k)` / `r.exists(k)` | DEL / EXISTS |
| `r.increment(k)` / `r.incrementBy(k, n)` / `r.decrement(k)` | INCR / INCRBY / DECR |
| `r.expire(k, seconds)` / `r.ttl(k)` / `r.persist(k)` | Expiry control |
| `r.keys(pattern)` | KEYS — avoid on large production databases |

### Hashes

`r.hashSet(k, field, v)`, `r.hashGet`, `r.hashDelete`, `r.hashExists`,
and `r.hashAll(k)` — which returns a **dictionary** rather than the flat
field/value array Redis sends.

### Lists

`r.listPush` (RPUSH), `r.listPushFront` (LPUSH), `r.listPop`,
`r.listPopFront`, `r.listLength`, `r.listRange(k, start, stop)`.

### Sets

`r.setAdd`, `r.setRemove`, `r.setMembers`, `r.setHas`, `r.setSize`.

### Server

`r.select(db)`, `r.auth(password)`, `r.authAs(user, password)`,
`r.flushDb()`, `r.info()`, `r.dbSize()`.

### Anything else

Every wrapper above is a thin call over one primitive, so commands this client
does not name are still reachable:

```ez
r.command(["ZADD", "leaderboard", "100", "ali"])
r.command(["ZRANGE", "leaderboard", "0", "-1", "WITHSCORES"])
```

## Errors

A Redis error reply means the command was wrong — a bug in the caller, not a
value to branch on — so `command()` **throws**:

```ez
try { r.command(["NOSUCHCOMMAND"]) }
catch (e) { out e.message }     # redis: ERR unknown command 'NOSUCHCOMMAND'
```

Where a failure is expected, `tryCommand` returns `{ "error": "..." }`
instead:

```ez
reply = r.tryCommand(["GET"])              # wrong arity
when respIsError(reply) { out reply["error"] }
```

A closed connection also throws. `isAlive()` is the way to check availability
without treating an unreachable server as an error.

## About the protocol

RESP is length-prefixed, which is why a value containing `\r\n` — or NUL
bytes, or anything binary — survives the wire intact.

The decoder returns "not yet" rather than guessing when a reply is
incomplete. TCP does not preserve message boundaries: a reply can arrive split
across reads, or two replies can arrive in one. Leftover bytes stay buffered
for the next call, so the stream never desynchronises.

## Limits

- **Not thread-safe.** One connection is one conversation; sharing a client
  across `spawn`ed threads will interleave replies. Use one client per thread.
- **No pub/sub, pipelining, transactions or cluster support.** All of these
  need a different read loop than request/response.
- **No TLS.** Connect over a private network, or tunnel it.
- `keys(pattern)` scans the whole keyspace and blocks the server — fine in
  development, bad in production.

## Testing

```
ez test.ez
```

39 protocol tests run without a server, covering encoding, every reply type,
nested arrays, nil handling, short reads and malformed input. A further 12
client tests run automatically when a Redis is reachable on
`127.0.0.1:6379`, and report as skipped otherwise.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Redis` client, command wrappers, URL connection |
| `resp.ez` | RESP encoding and incremental decoding |

## License

MIT
