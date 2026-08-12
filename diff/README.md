# diff — text and sequence diffing for EZ

> **Import:** `use "diff"`
> **Install:** `ez install diff`

```ez
use "diff"

out Diff.unified(before, after, "config.old", "config.new")
```

```diff
--- config.old
+++ config.new
@@ -1,3 +1,3 @@
 host = localhost
-port = 8080
+port = 9090
 debug = true
```

---

## Why LCS

Comparing line by line in order reports a single inserted line at the top as
*every line changed*. Finding the longest common subsequence first identifies
what genuinely stayed, so only the real edits are reported:

```ez
Diff.stats("a\nb\nc", "NEW\na\nb\nc")
# { added: 1, removed: 0, unchanged: 3 }
```

## API

### Comparing

| Call | Result |
| --- | --- |
| `Diff.lines(old, new)` | Edit script: `[{ op, value }]`, op is `keep`/`add`/`remove` |
| `Diff.sequences(a, b)` | The same over any two lists |
| `Diff.chars(a, b)` | The same over characters |
| `Diff.words(old, new)` | The same over words |
| `Diff.isSame(old, new)` | Whether anything changed |
| `Diff.stats(old, new)` | `{ added, removed, unchanged }` |

### Formatting

| Call | Result |
| --- | --- |
| `Diff.inline(old, new)` | Every line prefixed `+ `, `- ` or two spaces |
| `Diff.changesOnly(old, new)` | Only the changed lines |
| `Diff.unified(old, new, oldName, newName)` | Unified diff, 3 lines of context |
| `Diff.unifiedWith(..., context)` | Choose the context width |
| `Diff.wordsInline(old, new)` | `[-removed-]` and `{+added+}` inline |

Unified output is what `patch` and `git` speak. Changes closer together than
the context width merge into one hunk; further apart, they become separate
hunks with their own `@@` headers.

### Applying

```ez
script = Diff.lines(before, after)

Diff.apply(script)              # reproduces `after` exactly
Diff.apply(Diff.invert(script)) # back to `before`
```

`apply` reconstructing the target exactly is what makes an edit script
verifiable — it is the property the test suite leans on hardest.

## Details

**Line endings are normalised.** CRLF and CR both become LF before comparing,
so a file saved on Windows does not diff as entirely changed against the same
file saved on Linux.

**A trailing newline is not an empty last line.** Without that, every file
ending in a newline would report a spurious change.

**Ties favour removal.** When the LCS allows either direction, a replaced line
reads as removal then addition, which is the conventional order.

## Limits

- **Quadratic in time and memory.** The LCS table is `n × m` cells, so inputs
  over roughly 2000 × 2000 lines are **refused** with an error rather than
  allocating for minutes and running out of memory. Compare smaller pieces.
- **`Diff.chars` is for short strings.** The same quadratic cost applies per
  character, so it is the wrong tool for whole documents.
- **No fuzzy or patch-applying-to-different-source.** `apply` replays a script
  built from the exact same pair of inputs; it is not a `patch` implementation
  that can locate context in a drifted file.
- **No rename or move detection.**

## Testing

```
ez test.ez
```

63 tests. The most valuable ones assert that **every op appears exactly
once** — an earlier version emitted a spurious `keep` for every addition,
because in EZ a `when A {} when B {} other {}` chain is not a three-way
branch: the `other` attaches only to the last `when`, so matching the first
also ran the fallback. Those assertions pin that down.

Also covered: insertions and deletions reported minimally, unified hunk
headers and grouping, apply/invert round-trips, and the size guard.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Diff` model: formats, hunks, applying, inverting |
| `lcs.ez` | Longest common subsequence table and edit script |

## License

MIT
