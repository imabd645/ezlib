# stats

Descriptive statistics for EZ. Pure EZ, no dependencies.

```ez
use "stats"

mean([2, 4, 6])             # 4
median([1, 2, 3, 4])        # 2.5
mode([1, 2, 2, 3])          # 2
stddev([1, 2, 3, 4])        # sample standard deviation
percentile(values, 90)      # nearest-rank 90th percentile
linreg(xs, ys)              # { slope, intercept, r }
```

## API

| Function | Returns |
|----------|---------|
| `sum(values)` | total (0 for empty) |
| `mean(values)` | average, or `nil` |
| `median(values)` | middle value, or `nil` |
| `mode(values)` | most frequent value, or `nil` |
| `minimum(values)` / `maximum(values)` | extremum, or `nil` |
| `span(values)` | `max - min`, or `nil` |
| `variance(values, sample = true)` | variance, or `nil` |
| `stddev(values, sample = true)` | standard deviation, or `nil` |
| `percentile(values, p)` | nearest-rank percentile (`p` in 0..100) |
| `quantile(values, q)` | same, with `q` in 0..1 |
| `correlation(xs, ys)` | Pearson `r` in `[-1, 1]`, or `nil` |
| `linreg(xs, ys)` | `{ slope, intercept, r }` |

## Conventions

- Reductions return `nil` on empty input so "no data" is distinguishable from
  "a real zero". `sum` is the exception — `sum([])` is `0`.
- `variance`/`stddev` default to the **sample** form (divide by `n - 1`). Pass
  `false` for the population form (divide by `n`).
- `percentile` uses the nearest-rank method (the k-th sorted value, no
  interpolation). It is the simplest well-defined method; interpolated
  percentiles are a follow-on if you need them.

## Notes

- `median`, `percentile`, and `quantile` sort a copy, so the input list is not
  mutated.
- The function is called `span`, not `range`, because `range` is an interpreter
  builtin and a package must never shadow one.
- `correlation` returns `nil` (not 0) when a series is constant — the
  coefficient is genuinely undefined there, and silently returning 0 would
  dress up "no relationship" as "strong inverse relationship".
