# cache — in-memory caching for EZ

> **Import:** `use "cache"`
> **Install:** `ez install cache`

TTL expiry, LRU eviction, compute-on-miss and memoisation.

```ez
use "cache"

users = Cache(300)                       # entries live 300 seconds

profile = users.fetch("user:42", || loadProfile(42))
```

---

## Quick Start

```ez
use "cache"

c = Cache(60)             # default TTL in seconds; 0 means no expiry

c.set("key", value)
c.get("key")              # nil once expired
c.has("key")
c.remove("key")
```

**`fetch` is the operation you usually want.** It returns the cached value or
computes, stores and returns it — and unlike `get`, it has no ambiguity about
cached nils:

```ez
config = c.fetch("config", || YAML.load("config.yaml"))
```

## Expiry is lazy

Entries are checked when read, not swept on a timer — there is no background
scheduler here, and a lazy check costs nothing until the key is asked for.
Reading an expired entry also removes it, so a read-heavy workload cleans up
as it goes.

For a workload that writes keys it never reads again, call `prune()`
periodically, or set a `limit()` so eviction bounds the size regardless.

## API

### Construction

| Call | Result |
| --- | --- |
| `Cache(ttlSeconds)` | A cache with a default TTL; `0` for no expiry |
| `c.limit(maxEntries)` | Bound the size, evicting least-recently-used. Returns the cache |
| `lruCache(maxEntries)` | Shorthand: no expiry, bounded by count |

### Storing and reading

| Call | Result |
| --- | --- |
| `c.set(key, value)` | Store with the default TTL. Returns the value |
| `c.setFor(key, value, ttl)` | Store with a per-entry TTL |
| `c.get(key)` | Value, or nil when absent or expired |
| `c.getOr(key, fallback)` | Value, or `fallback` |
| `c.has(key)` | Whether a live entry exists |
| `c.fetch(key, producer)` | Cached value, else call `producer`, store, return |
| `c.fetchFor(key, ttl, producer)` | As `fetch`, with an explicit TTL |
| `c.remove(key)` | Whether something was removed |
| `c.clear()` | Empty the cache |

Keys are converted to strings, so `1` and `"1"` are the same entry.

> **nil is a storable value.** `get` returns nil for both "cached nil" and
> "not cached". Use `has()` to tell them apart — or `fetch()`, which does not
> have the problem: a cached nil is a hit and the producer does not re-run.

### Inspecting

| Call | Result |
| --- | --- |
| `c.keys()` | Live keys, least-recently-used first |
| `c.size()` | Number of live entries |
| `c.prune()` | Drop expired entries now; returns how many |
| `c.stats()` | `{ hits, misses, evictions, size, hitRate }` |
| `c.resetStats()` | Zero the counters |

`stats()` is there so you can tell whether the cache is earning its place. A
hit rate near zero means the TTL is too short or the keys are too specific.

### Memoisation

```ez
fast = memoize(|n| expensiveCalculation(n), 0)

fast(10)      # computes
fast(10)      # cached
```

`memoize(fn, ttlSeconds)` wraps a single-argument task. The argument is used
as the cache key via `str()`, so it works for numbers and strings; for
structured arguments, cache explicitly with a key you build yourself.

## LRU eviction

With a `limit()`, the least recently used entry is dropped when the cache is
full. **Reading counts as use** — an entry you keep reading survives:

```ez
c = lruCache(2)
c.set("a", 1)
c.set("b", 2)
c.get("a")        # 'a' is now most recently used
c.set("c", 3)     # evicts 'b', not 'a'
```

This matters for a long-running server: without a limit, a cache keyed on
anything user-supplied grows without bound.

## Notes

- **Not thread-safe.** Sharing one cache across `spawn`ed threads without a
  mutex will corrupt it. Use one cache per thread, or guard it.
- TTLs are in seconds and may be fractional (`Cache(0.5)`).
- Expiry uses `clock()`, so it tracks real elapsed time.

## Testing

```
ez test.ez
```

50 tests covering storage, real-time expiry, per-entry TTLs, LRU ordering,
fetch semantics, statistics and memoisation.

## License

MIT
