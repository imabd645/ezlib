# ezmath — Mathematics Library for EZ

> **Version:** 3.0  
> **Import:** `use "ezmath"`  
> **File:** `E:\ezlib\ezmath\main.ez`

---

## Overview

`ezmath` is a comprehensive mathematics library for EZ, providing everything from basic trigonometry to statistics, number theory, vector math, random number generation, and interpolation. All algorithms are implemented entirely in pure EZ (no external DLLs).

After `use "ezmath"`, all functions are available **globally** and also accessible through the `math` dictionary object.

---

## Quick Start

```ez
use "ezmath"

out sin(PI / 2)       # → 1.0
out cos(0)            # → 1.0
out ln(E)             # → 1.0
out isPrime(17)       # → true
out randInt(1, 10)    # → random integer 1–10

v = Vector2(3, 4)
out v.length()        # → 5.0
```

---

## Constants

| Name | Value | Description |
|---|---|---|
| `PI` | `3.141592653589793` | π |
| `E` | `2.718281828459045` | Euler's number |
| `PHI` | `1.618033988749895` | Golden ratio |
| `SQRT2` | `1.4142135623730951` | √2 |
| `LN2` | `0.6931471805599453` | Natural log of 2 |
| `LN10` | `2.302585092994046` | Natural log of 10 |

Also accessible as `math["PI"]`, `math["E"]`, etc.

---

## Trigonometry

All functions work in **radians**. Use `degToRad()` / `radToDeg()` for degree conversion.

### `sin(x)` → `number`
Sine of x (radians). Computed as `cos(π/2 - x)`.

### `cos(x)` → `number`
Cosine of x (radians). Taylor series (6 terms, normalized to `[-π, π]`).

### `tan(x)` → `number`
Tangent of x. Throws `"Division by zero in tan()"` when `cos(x) ≈ 0`.

### `asin(x)` → `number`
Arcsine. Domain: `[-1, 1]`. Throws on out-of-domain input.

### `acos(x)` → `number`
Arccosine. Implemented as `π/2 - asin(x)`.

### `atan(x)` → `number`
Arctangent. Uses range reduction for `|x| > 1`.

### `atan2(y, x)` → `number`
Two-argument arctangent. Handles all four quadrants. Throws on `(0, 0)`.

### `sinh(x)` / `cosh(x)` / `tanh(x)` → `number`
Hyperbolic functions using `pow(E, x)`.

### Degree Variants
| Function | Description |
|---|---|
| `sind(d)` | Sine of degrees |
| `cosd(d)` | Cosine of degrees |
| `tand(d)` | Tangent of degrees |
| `asind(x)` | Arcsine → degrees |
| `acosd(x)` | Arccosine → degrees |
| `atand(x)` | Arctangent → degrees |
| `atan2d(y, x)` | Two-arg atan → degrees |

```ez
use "ezmath"

out sind(30)    # → 0.5
out cosd(60)    # → 0.5
out tand(45)    # → ~1.0
out asind(0.5)  # → 30.0
out atan2d(1, 1)  # → 45.0
```

### Angle Conversion

| Function | Description |
|---|---|
| `degToRad(d)` | Degrees → Radians |
| `radToDeg(r)` | Radians → Degrees |
| `degToGrad(d)` | Degrees → Gradians |
| `gradToDeg(g)` | Gradians → Degrees |
| `radToGrad(r)` | Radians → Gradians |
| `gradToRad(g)` | Gradians → Radians |

```ez
use "ezmath"

out degToRad(180)   # → 3.14159...
out radToDeg(PI)    # → 180.0
```

---

## Logarithms & Exponentials

### `ln(x)` → `number`
Natural logarithm. Uses range reduction (divides/multiplies by 2 to bring into `[1,2)`) then Taylor series.
Throws on `x <= 0`.

### `log10(x)` → `number`
Base-10 logarithm. `ln(x) / LN10`.

### `log2(x)` → `number`
Base-2 logarithm. `ln(x) / LN2`.

### `log(base, x)` → `number`
Arbitrary base logarithm. `ln(x) / ln(base)`.

### `exp(x)` → `number`
e^x. Uses split integer/fractional computation for stability.

```ez
use "ezmath"

out ln(1)         # → 0
out ln(E)         # → 1.0
out log10(1000)   # → 3.0
out log2(8)       # → 3.0
out log(3, 27)    # → 3.0
out exp(1)        # → 2.71828...
out exp(0)        # → 1
```

---

## Rounding & Precision

```ez
use "ezmath"

out round(3.5)    # → 4
out round(3.4)    # → 3
out round(-3.5)   # → -4
out trunc(3.9)    # → 3
out trunc(-3.9)   # → -3
out sign(5)       # → 1
out sign(-3)      # → -1
out sign(0)       # → 0
out frac(3.75)    # → 0.75
```

---

## Random Number Generation

Uses a **Linear Congruential Generator** (LCG) seeded from `clock()` at load time.

### `randInt(min, max)` → `integer`
Returns a random integer in the range `[min, max]` (inclusive).

### `randFloat(min, max)` → `float`
Returns a random float in `[min, max)`.

### `randBool()` → `boolean`
Returns `true` or `false` randomly.

### `randChoice(arr)` → `value`
Returns a random element from an array.

### `randNormal(mean, stddev)` → `float`
Returns a normally distributed random number using the **Box-Muller transform**.

### `seedRand(s)`
Re-seeds the RNG with a specific value for reproducible sequences.

```ez
use "ezmath"

out randInt(1, 6)      # → 1–6 (dice roll)
out randFloat(0, 1)    # → 0.000..–0.999..
out randBool()         # → true or false
out randChoice(["a", "b", "c"])  # → random element
out randNormal(0, 1)   # → normally distributed float

# Reproducible sequence
seedRand(42)
out randInt(1, 100)    # Always the same for seed 42
```

---

## Number Theory

### `gcd(a, b)` → `number`
Greatest Common Divisor using Euclidean algorithm.

### `lcm(a, b)` → `number`
Least Common Multiple. Returns `0` if either input is `0`.

### `isPrime(n)` → `boolean`
Primality test using trial division up to `√n`.

### `nextPrime(n)` → `number`
Returns the next prime number after `n`.

### `factors(n)` → `array`
Returns the prime factorization as an array.

### `fibonacci(n)` → `number`
Returns the nth Fibonacci number (iterative, 0-indexed: `fibonacci(0) = 0, fibonacci(1) = 1`).

### `isEven(n)` / `isOdd(n)` → `boolean`

### `isPowerOf2(n)` → `boolean`
Uses bitwise check: `n & (n-1) == 0`.

### `factorial(n)` → `number`
Recursive factorial. `factorial(0) = 1`.

```ez
use "ezmath"

out gcd(48, 18)        # → 6
out lcm(4, 6)          # → 12
out isPrime(17)        # → true
out isPrime(18)        # → false
out nextPrime(10)      # → 11
out factors(12)        # → [2, 2, 3]
out fibonacci(10)      # → 55
out isEven(4)          # → true
out isPowerOf2(16)     # → true
out factorial(5)       # → 120
```

---

## Array Math Utilities

```ez
use "ezmath"

data = [3, 1, 4, 1, 5, 9, 2, 6]

out sum(data)          # → 31
out product(data)      # → 6480
out min(data)          # → 1
out max(data)          # → 9
out average(data)      # → 3.875
out range(data)        # → 8 (max - min)

mm = minMax(data)
out mm["min"]          # → 1
out mm["max"]          # → 9
```

---

## Interpolation & Easing

### `lerp(a, b, t)` → `number`
Linear interpolation between `a` and `b`. `t` is clamped to `[0, 1]`.

### `clamp(v, min, max)` → `number`
Clamps `v` to `[min, max]`.

### `smoothstep(edge0, edge1, x)` → `number`
Smooth Hermite interpolation (cubic).

### `smootherstep(edge0, edge1, x)` → `number`
Smoother 5th-order Hermite interpolation.

### `step(edge, x)` → `0 | 1`
Returns `0` if `x < edge`, else `1`.

### `map(value, inMin, inMax, outMin, outMax)` → `number`
Remaps a value from one range to another.

### `wrapAngle(angle)` → `number`
Wraps an angle to `[-π, π]`.

### Easing Functions
| Function | Description |
|---|---|
| `easeInQuad(t)` | Quadratic ease-in |
| `easeOutQuad(t)` | Quadratic ease-out |
| `easeInOutQuad(t)` | Quadratic ease-in-out |
| `easeInCubic(t)` | Cubic ease-in |
| `easeOutCubic(t)` | Cubic ease-out |
| `easeInOutCubic(t)` | Cubic ease-in-out |
| `easeInSine(t)` | Sine ease-in |
| `easeOutSine(t)` | Sine ease-out |
| `easeInOutSine(t)` | Sine ease-in-out |

```ez
use "ezmath"

out lerp(0, 100, 0.5)         # → 50
out lerp(0, 100, 0.25)        # → 25
out clamp(150, 0, 100)        # → 100
out smoothstep(0, 1, 0.5)     # → 0.5
out map(5, 0, 10, 0, 100)     # → 50
out easeInQuad(0.5)           # → 0.25
out easeOutQuad(0.5)          # → 0.75
```

---

## Distance Functions

```ez
use "ezmath"

out dist2D(0, 0, 3, 4)        # → 5.0 (3-4-5 triangle)
out dist3D(0,0,0, 1,1,1)      # → ~1.732
out manhattan2D(0,0, 3,4)     # → 7

out angleBetween(0,0, 1,1)    # → ~0.785 (45 degrees in radians)

pt = pointOnCircle(0, 0, 5, PI/4)
out pt["x"]   # → ~3.535
out pt["y"]   # → ~3.535
```

---

## Model: `Vector2`

2D vector with math operations.

```ez
use "ezmath"

a = Vector2(3, 4)
b = Vector2(1, 2)

out a.length()          # → 5.0
out a.dot(b)            # → 11
out a.distanceTo(b)     # → ~2.83
out a.angleTo(b)        # → radians

# Arithmetic — returns new Vector2
c = a.add(b)            # → Vector2(4, 6)
d = a.sub(b)            # → Vector2(2, 2)
e = a.mul(2)            # → Vector2(6, 8)
f = a.div(2)            # → Vector2(1.5, 2)

# Normalize (mutates and returns self)
n = Vector2(3, 4)
n.normalize()
out n.x   # → 0.6
out n.y   # → 0.8

# Lerp (returns new Vector2)
mid = a.lerp(b, 0.5)
out mid.x   # → 2
out mid.y   # → 3

a.print()  # → Vec2(3, 4)
```

---

## Model: `Vector3`

3D vector with cross product.

```ez
use "ezmath"

a = Vector3(1, 0, 0)
b = Vector3(0, 1, 0)

cross = a.cross(b)   # → Vector3(0, 0, 1)
cross.print()
out a.dot(b)   # → 0 (perpendicular)
out a.length() # → 1.0

v = Vector3(3, 4, 0)
v.normalize()
v.print()  # → Vec3(0.6, 0.8, 0)
```

---

## Model: `Matrix2x2`

2x2 matrix with determinant, inverse, and vector multiplication.

```ez
use "ezmath"

m = Matrix2x2(1, 2, 3, 4)
out m.determinant()   # → -2

inv = m.inverse()     # → Matrix2x2 inverse

v = Vector2(5, 6)
result = m.mulVector(v)
result.print()  # → Vec2(17, 39)
```

---

## Model: `Statistics`

Statistical analysis of a numeric dataset.

```ez
use "ezmath"

data = [2, 4, 4, 4, 5, 5, 7, 9]
s = Statistics(data)  # or use: s = stats(data)

out s.count()      # → 8
out s.sum()        # → 40
out s.mean()       # → 5.0
out s.median()     # → 4.5
out s.mode()       # → 4
out s.min()        # → 2
out s.max()        # → 9
out s.range()      # → 7
out s.variance()   # → 4.0
out s.stddev()     # → 2.0
out s.sem()        # → ~0.707

# Percentiles
out s.percentile(25)   # → Q1
out s.percentile(75)   # → Q3

q = s.quartiles()
out q["q1"]   # → 4
out q["q3"]   # → 6
out q["iqr"]  # → 2

summary = s.summary()
out summary["mean"]  # → 5.0
```

### Correlation

```ez
use "ezmath"

s1 = Statistics([1, 2, 3, 4, 5])
s2 = Statistics([2, 4, 6, 8, 10])

out s1.covariance(s2)    # → 5.0
out s1.correlation(s2)   # → 1.0 (perfect positive correlation)
```

---

## Utility Functions

```ez
use "ezmath"

out nearlyEqual(0.1 + 0.2, 0.3)           # → true (epsilon = 0.000001)
out nearlyEqual(0.1 + 0.2, 0.3, 0.0001)  # → true (custom epsilon)
out isFinite(1 / 0)   # → false
out isNaN(0 / 0)      # → may behave unexpectedly (EZ-specific)
```

---

## Edge Cases

- `tan()` at `π/2` (90°) throws — check `cos(x) ≈ 0` before calling.
- `asin(x)` / `acos(x)` require `|x| ≤ 1` — throws otherwise.
- `min()`, `max()`, `average()` throw on empty arrays.
- `Statistics.covariance()` requires equal-length datasets.
- `ln(0)` and `ln(negative)` throw.
- `Matrix2x2.inverse()` throws if determinant is near zero (singular matrix).
- `randNormal()` uses `ln()` internally — if `u1 = 0` (theoretically possible), it would throw.

---

## Full Example: Physics Simulation Step

```ez
use "ezmath"

# Simple projectile motion
g = 9.8      # gravity m/s²
v0 = 50.0    # initial speed m/s
angle = 45.0 # degrees

vx = v0 * cosd(angle)
vy = v0 * sind(angle)
t = 0
dt = 0.1

out "Projectile trajectory:"
while vy - g*t >= 0 {
    x = vx * t
    y = vy * t - 0.5 * g * t * t
    out "  t=" + str(round(t * 10) / 10) + "s  x=" + str(round(x)) + "m  y=" + str(round(y)) + "m"
    t = t + dt
}
```

---

*Documentation generated from `E:\ezlib\ezmath\main.ez` — EZ Math Library v3.0*
