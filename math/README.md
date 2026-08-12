# EZ Math

`math` is the production-oriented mathematics package for EZ. Import it with:

```ez
use "math"
```

It has no external dependencies and exports its API into the importing module.

## Layout

| File | Responsibility |
|---|---|
| `src/constants.ez` | Mathematical constants and tolerance |
| `src/core.ez` | Conversion, rounding, comparisons, and array aggregates |
| `src/trigonometry.ez` | Trigonometry, inverse trigonometry, logs, and exponentials |
| `src/number_theory.ez` | Integer algorithms and prime utilities |
| `src/statistics.ez` | `Statistics` and descriptive statistics |
| `src/interpolation.ez` | Mapping, interpolation, steps, and easing |
| `src/random.ez` | Seedable pseudo-random sampling |
| `src/geometry.ez` | Distance and circle helpers |
| `src/linear_algebra.ez` | Vectors and 2×2 matrices |

`main.ez` is intentionally only an import surface. Consumers must use `math`,
not a `src/` implementation file, to keep future refactors compatible.

## Numeric API

- Constants: `PI`, `TAU`, `E`, `PHI`, `SQRT2`, `LN2`, `LN10`, `EPSILON`
- Conversion: `degToRad`, `radToDeg`, `degToGrad`, `gradToDeg`, `radToGrad`, `gradToRad`
- Core: `round`, `trunc`, `sign`, `frac`, `clamp`, `nearlyEqual`, `isNaN`, `isFinite`
- Aggregates: `sum`, `product`, `min`, `max`, `minMax`, `average`, `range`

`round` rounds half values away from zero. Aggregate functions require a non-empty
array except `sum` (zero) and `product` (one).

## Trigonometry and logarithms

`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, and `atan2` use radians. Degree
variants are suffixed with `d`, for example `sind(30)` and `atan2d(1, 1)`.

`ln`, `log2`, `log10`, `log`, and `exp` validate their mathematical domains.
`atan2(0, 0)`, `tan` at an undefined angle, invalid logarithm bases, and inverse
trigonometric values outside `[-1, 1]` raise errors instead of returning a
plausible but incorrect number.

## Integer algorithms

```ez
out gcd(48, 18)          # 6
out lcm(12, 18)          # 36
out factors(360)         # [2, 2, 2, 3, 3, 5]
out fibonacci(10)        # 55
out isPrime(97)          # true
```

`factorial`, `fibonacci`, and `factors` reject invalid integer domains.

## Statistics

```ez
s = Statistics([2, 4, 4, 4, 5, 5, 7, 9])
out s.mean()             # 5
out s.median()           # 4.5
out s.stddev()           # 2
out s.percentile(75)     # 5.5
```

`Statistics` copies its input array. Empty datasets raise for measurements
whose value is undefined; `mode()` returns `nil` for an empty dataset.

## Randomness

`seedRand`, `randInt`, `randFloat`, `randBool`, `randChoice`, and `randNormal`
use a deterministic LCG. It is appropriate for games, tests, and simulations;
it is **not cryptographically secure**. Use `secure_token` from the web package
or platform cryptography for secrets.

## Linear algebra

`Vector2`, `Vector3`, and `Matrix2x2` provide immutable arithmetic results;
`normalize()` mutates the vector and returns it. Normalizing a zero vector,
dividing a vector by zero, and inverting a singular matrix raise errors.

## Testing

Run the standalone suite with an EZ interpreter that can resolve `C:/ezlib`:

```powershell
ez C:\ezlib\math\test_math.ez
```

