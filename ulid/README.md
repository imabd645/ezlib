# ulid — sortable unique identifiers for EZ

> **Import:** `use "ulid"`
> **Install:** `ez install ulid`

26 characters: a 48-bit millisecond timestamp followed by 80 bits of
randomness, in Crockford base32.

```ez
use "ulid"

id = ulid()      # 01ARZ3NDEKTSV4RRFFQ69G5FAV
```

---

## Why not a random UUID

**IDs generated later sort after IDs generated earlier, as plain text.** A
random UUID has no order at all, so using one as a primary key scatters
inserts across a B-tree index and fragments it; a ULID appends. It also means
you can sort records by ID and get them chronologically, without a separate
timestamp column.

```ez
ulidAt(1000) < ulidAt(2000)     # true — plain string comparison
```

The timestamp is fixed-width for exactly this reason: a variable-width one
would make `999999` sort after `1000000`.

## API

### Generating

| Call | Result |
| --- | --- |
| `ulid()` | A new ULID for now |
| `ulidAt(timeMs)` | A ULID for a specific millisecond |
| `ulidFloor(timeMs)` / `ulidCeiling(timeMs)` | Smallest/largest ULID for a millisecond |

`ulidAt` is for seeding fixtures with a known order, or backfilling rows that
already have a timestamp.

### Reading

| Call | Result |
| --- | --- |
| `isUlid(text)` | Whether it is a valid ULID |
| `ulidTimeOf(id)` | The millisecond encoded in it |
| `ulidRandomOf(id)` | The 16-character random part |
| `ulidCanonical(text)` | Normalised: uppercased, `I`/`L` to `1`, `O` to `0` |
| `ulidCompare(a, b)` / `ulidSort(list)` | Ordering helpers |

`ulidTimeOf` and `ulidRandomOf` throw on a non-ULID rather than returning
nonsense.

## Monotonic within a millisecond

Two IDs made in the same tick would otherwise be ordered at random. Instead of
redrawing the random part, it is **incremented**, so creation order still
holds:

```ez
ids = []
repeat i = 1 to 100 { push(ids, ulid()) }
# every id sorts strictly after the one before it
```

A new millisecond redraws the randomness.

## The alphabet

Crockford base32 omits `I`, `L`, `O` and `U` — the first three because they
are easily confused with `1` and `0`, and `U` to avoid accidental
obscenities. Decoding **accepts the confusable letters anyway** and maps them
to the digit they resemble, so an ID copied by hand still resolves:

```ez
ulidCanonical("0IARZ3NDEKTSV4RRFFQ69G5FAV")   # the I becomes 1
```

## Range queries

Because ULIDs sort chronologically, a time range is a string range:

```ez
lower = ulidFloor(dayStart)
upper = ulidCeiling(dayEnd)
# SELECT * FROM events WHERE id >= lower AND id <= upper
```

No index on a timestamp column required.

## Notes

- **Not cryptographically secure.** The randomness comes from `randint`, which
  is fine for uniqueness but not for anything that must be unguessable. Use
  `crypto` for tokens and session identifiers.
- **The timestamp is visible.** Anyone holding an ID knows when it was
  created — usually fine, occasionally not.
- Valid until the year 10889, when 48 bits of milliseconds run out.

## Testing

```
ez test.ez
```

45 tests covering the alphabet, timestamp round-tripping, sort order across
digit counts, monotonicity within a millisecond, uniqueness over 500 IDs,
canonicalisation and range bounds. Includes the specification's own worked
example.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Generation, parsing, comparison and range bounds |
| `crockford.ez` | Fixed-width base32 encoding and tolerant decoding |

## License

MIT
