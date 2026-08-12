# semver — semantic versioning for EZ

> **Import:** `use "semver"`
> **Install:** `ez install semver`

Parse, compare and match semantic versions and version ranges.

```ez
use "semver"

Semver.compare("1.10.0", "1.9.0")        # 1  — numeric, not lexical
Semver.satisfies("1.4.2", "^1.2.0")      # true
Semver.maxSatisfying(available, "^1.0.0")
```

---

## The two things this gets right

**`1.10.0` is newer than `1.9.0`.** Comparing versions as strings puts `1.10.0`
*before* `1.9.0`, because `"1"` sorts before `"9"`. Each part is compared as a
number here.

**`1.0.0-beta` is older than `1.0.0`.** A prerelease precedes its release, and
prerelease identifiers compare per semver.org: numeric ones numerically, and
numeric ranks below alphanumeric. `1.0.0-alpha < 1.0.0-beta < 1.0.0`.

## API

### Parsing

| Call | Result |
| --- | --- |
| `Semver.parse(v)` | `{ major, minor, patch, prerelease, build }`, or nil |
| `Semver.isValid(v)` | Whether it parses |
| `Semver.clean(v)` | `"v1.2.3-beta+build"` → `"1.2.3"` |
| `Semver.isPrerelease(v)` | Whether it has a prerelease part |

A leading `v` is accepted, since tags are so often written that way. Leading
zeroes (`01.0.0`) are rejected — otherwise `01` and `1` would be two spellings
of one version.

### Comparing

| Call | Result |
| --- | --- |
| `Semver.compare(a, b)` | `-1`, `0` or `1` |
| `Semver.gt / lt / gte / lte / eq / neq` | Booleans |
| `Semver.diff(a, b)` | `"major"`, `"minor"`, `"patch"`, `"prerelease"` or nil |

`compare` **throws** on an unparseable version — silently ordering garbage is
worse than stopping. `satisfies` returns `false` instead, since a range check
is a question, not an assertion.

Build metadata is ignored when comparing, as the spec requires: `1.0.0+a` and
`1.0.0+b` are equal.

### Ranges

| Call | Result |
| --- | --- |
| `Semver.satisfies(v, range)` | Whether the version matches |
| `Semver.maxSatisfying(list, range)` | Highest match, or nil |
| `Semver.minSatisfying(list, range)` | Lowest match, or nil |

| Range | Matches |
| --- | --- |
| `1.2.3` | Exactly that version |
| `^1.2.3` | `>=1.2.3` with no major change → `1.2.3` … `1.x.x` |
| `^0.2.3` | `0.2.x` only — for 0.x, minor is treated as breaking |
| `~1.2.3` | `1.2.x` only |
| `>=1.2.3` `>1.2.3` `<=1.2.3` `<1.2.3` `=1.2.3` | Comparators |
| `1.2.x` `1.x` `*` | Wildcards |
| `1.2.3 - 2.0.0` | Inclusive interval |
| `>=1.0.0 <2.0.0` | Space-separated: all must hold |
| `^1.0.0 \|\| ^2.0.0` | Either |

> **Prereleases do not satisfy plain ranges.** `2.0.0-alpha` does not match
> `*` or `^1.0.0`, matching how package managers behave — nobody wants an
> alpha pulled in by a caret range. To opt in, mention a prerelease in the
> range: `>=1.0.0-alpha`.

### Sorting

| Call | Result |
| --- | --- |
| `Semver.sort(list)` | Ascending |
| `Semver.rsort(list)` | Descending |
| `Semver.max(list)` / `Semver.min(list)` | Ends of the range |

Entries that do not parse are **dropped** rather than ordered arbitrarily.

### Incrementing

```ez
Semver.inc("1.2.3", "major")     # 2.0.0
Semver.inc("1.2.3", "minor")     # 1.3.0
Semver.inc("1.2.3", "patch")     # 1.2.4
Semver.inc("1.2.3-beta", "patch") # 1.2.3  — a prerelease releases itself
```

Incrementing clears the parts below it, plus any prerelease and build
metadata. An unknown part name throws.

## Examples

**Pick the best available release:**

```ez
available = ["1.0.0", "1.5.0", "1.9.2", "2.0.0"]
chosen = Semver.maxSatisfying(available, "^1.0.0")     # 1.9.2
```

**Decide whether an upgrade is safe:**

```ez
when Semver.diff(current, latest) == "patch" {
    installUpdate(latest)
}
```

**Guard a minimum version:**

```ez
when Semver.lt(runtimeVersion, "4.0.0") {
    throw Exception("this package needs EZ 4.0.0 or newer")
}
```

## Testing

```
ez test.ez
```

89 tests covering parsing, ordering (including prerelease rules), every range
form, selection, sorting and incrementing.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | Public `Semver` model |
| `parse.ez` | Version text to parts; comparison and prerelease ordering |
| `range.ez` | Range matching: caret, tilde, wildcards, intervals, unions |

## License

MIT
