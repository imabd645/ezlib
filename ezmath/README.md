# ezmath

> Comprehensive mathematical library for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**ezmath** provides everything from basic trig and logarithms to vectors, matrices, statistics, number theory, random generation, and easing functions — all implemented in pure EZ with no external dependencies.

---

## Installation

```
ez install math
```

---

## Quick Start

```ez
use "math"

out math.sin(math.PI / 2)       # 1.0
out math.log10(1000)             # 3.0
out math.isPrime(97)             # true
out math.fibonacci(10)           # 55

v = Vector2(3, 4)
out v.length()                   # 5.0

s = Statistics([4, 8, 15, 16, 23, 42])
out s.mean()                     # 18.0
out s.stddev()                   # ~13.0
```

---

## Constants

```ez
math.PI      # 3.141592653589793
math.E       # 2.718281828459045
math.PHI     # 1.618033988749895  (golden ratio)
math.SQRT2   # 1.4142135623730951
math.LN2     # 0.6931471805599453
math.LN10    # 2.302585092994046
```

---

## Trigonometry

All standard trig functions are available in both **radians** and **degrees**.

### Radians

```ez
math.sin(x)        # sine
math.cos(x)        # cosine
math.tan(x)        # tangent
math.asin(x)       # arcsine
math.acos(x)       # arccosine
math.atan(x)       # arctangent
math.atan2(y, x)   # two-argument arctangent
```

### Degrees (`d` suffix)

```ez
math.sind(90)      # 1.0
math.cosd(0)       # 1.0
math.tand(45)      # ~1.0
math.asind(1)      # 90.0
math.acosd(1)      # 0.0
math.atand(1)      # 45.0
math.atan2d(1, 1)  # 45.0
```

### Hyperbolic

```ez
math.sinh(x)
math.cosh(x)
math.tanh(x)
```

### Conversion

```ez
degToRad(180)    # 3.14159...
radToDeg(math.PI)  # 180.0
degToGrad(90)    # 100.0
radToGrad(x)
gradToDeg(x)
gradToRad(x)
```

---

## Logarithms & Exponentials

```ez
math.ln(x)         # natural log (base e)
math.log10(x)      # log base 10
math.log2(x)       # log base 2
math.log(b, x)     # log base b

math.exp(x)        # e^x

# Examples
math.ln(math.E)    # 1.0
math.log10(1000)   # 3.0
math.log2(256)     # 8.0
math.exp(1)        # ~2.71828
```

---

## Rounding & Precision

```ez
math.round(3.7)      # 4
math.trunc(3.9)      # 3  (truncate toward zero)
math.sign(-5)        # -1
math.sign(0)         # 0
math.sign(3)         # 1
math.frac(3.75)      # 0.75

nearlyEqual(0.1 + 0.2, 0.3)        # true (default epsilon: 0.000001)
nearlyEqual(a, b, 0.01)            # custom epsilon

isFinite(1 / 0)     # false
isNaN(0 / 0)        # true (when applicable)
```

---

## Random Number Generation

Seeds automatically from the system clock. Can be manually seeded for reproducibility.

```ez
math.seedRand(42)                   # set seed for reproducible output

math.randInt(1, 100)                # random integer in [1, 100]
math.randFloat(0.0, 1.0)           # random float in [0.0, 1.0]
math.randBool()                     # true or false
math.randChoice(["a", "b", "c"])   # random element from array
math.randNormal(0, 1)               # normal distribution (Box-Muller)
```

---

## Number Theory

```ez
math.gcd(48, 18)           # 6
math.lcm(4, 6)             # 12
math.isPrime(97)           # true
math.isPrime(100)          # false
math.nextPrime(10)         # 11
math.factors(60)           # [2, 2, 3, 5]
math.fibonacci(10)         # 55
math.isEven(4)             # true
math.isOdd(7)              # true
math.isPowerOf2(64)        # true
factorial(5)               # 120
```

---

## Array Math

```ez
data = [3, 1, 4, 1, 5, 9, 2, 6]

math.sum(data)             # 31
math.product([1,2,3,4])    # 24
math.min(data)             # 1
math.max(data)             # 9
math.minMax(data)          # { "min": 1, "max": 9 }
math.average(data)         # 3.875
math.range(data)           # 8  (max - min)
```

---

## Interpolation & Easing

### Core interpolation

```ez
math.lerp(0, 100, 0.25)             # 25.0
math.clamp(150, 0, 100)             # 100
math.map(0.5, 0, 1, 0, 360)         # 180.0  (remap range)
math.smoothstep(0, 1, 0.5)          # 0.5 (smooth S-curve)
math.smootherstep(0, 1, 0.5)        # 0.5 (smoother S-curve)
math.step(0.5, 0.3)                  # 0  (threshold step)
math.wrapAngle(4 * math.PI)          # wraps to [-PI, PI]
```

### Easing functions

```ez
# Quadratic
math.easeInQuad(t)
math.easeOutQuad(t)
math.easeInOutQuad(t)

# Cubic
math.easeInCubic(t)
math.easeOutCubic(t)
math.easeInOutCubic(t)

# Sine
math.easeInSine(t)
math.easeOutSine(t)
math.easeInOutSine(t)
```

All easing functions take `t` in the range `[0, 1]` and return a value in `[0, 1]`.

---

## Distance & Geometry

```ez
dist2D(0, 0, 3, 4)                  # 5.0
dist3D(0, 0, 0, 1, 2, 2)            # 3.0
manhattan2D(0, 0, 3, 4)             # 7

angleBetween(0, 0, 1, 1)            # PI/4 (radians)

# Get a point on a circle
p = pointOnCircle(0, 0, 5, math.PI / 4)
out p["x"]   # ~3.535
out p["y"]   # ~3.535
```

---

## Vector2

```ez
use "math"

a = Vector2(3, 4)
b = Vector2(1, 2)

out a.length()             # 5.0
out a.dot(b)               # 11.0
out a.distanceTo(b)        # ~2.83
out a.angleTo(b)           # angle in radians

# Arithmetic (returns new Vector2)
c = a.add(b)               # (4, 6)
c = a.sub(b)               # (2, 2)
c = a.mul(2)               # (6, 8)
c = a.div(2)               # (1.5, 2.0)

# Interpolate
mid = a.lerp(b, 0.5)       # (2, 3)

# Normalize in place
a.normalize()
out a.length()             # ~1.0

a.print()                  # Vec2(x, y)
```

### Vector2 Method Reference

| Method | Returns | Description |
|---|---|---|
| `.length()` | float | Magnitude of the vector |
| `.normalize()` | self | Scales vector to unit length |
| `.dot(v)` | float | Dot product with another vector |
| `.distanceTo(v)` | float | Distance to another vector |
| `.angleTo(v)` | float | Angle to another vector (radians) |
| `.lerp(v, t)` | Vector2 | Linear interpolation |
| `.add(v)` | Vector2 | Component-wise addition |
| `.sub(v)` | Vector2 | Component-wise subtraction |
| `.mul(s)` | Vector2 | Scalar multiplication |
| `.div(s)` | Vector2 | Scalar division |
| `.print()` | — | Prints `Vec2(x, y)` |

---

## Vector3

```ez
use "math"

a = Vector3(1, 2, 3)
b = Vector3(4, 5, 6)

out a.length()              # ~3.74
out a.dot(b)                # 32.0

cross = a.cross(b)          # Vector3(-3, 6, -3)
cross.print()               # Vec3(-3, 6, -3)

mid = a.lerp(b, 0.5)        # Vector3(2.5, 3.5, 4.5)
a.normalize()
```

All `Vector2` arithmetic and utility methods are also available on `Vector3`, plus `.cross(v)`.

---

## Matrix2x2

```ez
use "math"

m = Matrix2x2(1, 2, 3, 4)

out m.determinant()         # -2.0

inv = m.inverse()

v = Vector2(1, 0)
result = m.mulVector(v)     # transforms the vector by the matrix
result.print()
```

| Method | Returns | Description |
|---|---|---|
| `.determinant()` | float | Matrix determinant |
| `.inverse()` | Matrix2x2 | Inverse matrix (throws if singular) |
| `.mulVector(v)` | Vector2 | Transform a Vector2 by this matrix |

---

## Statistics

```ez
use "math"

s = Statistics([4, 8, 15, 16, 23, 42])

out s.count()              # 6
out s.sum()                # 108
out s.mean()               # 18.0
out s.median()             # 15.5
out s.mode()               # 4  (first most frequent)
out s.min()                # 4
out s.max()                # 42
out s.range()              # 38
out s.variance()           # ~176.0
out s.stddev()             # ~13.27
out s.sem()                # standard error of the mean
out s.percentile(75)       # 75th percentile

q = s.quartiles()
out q["q1"]                # 25th percentile
out q["q2"]                # 50th percentile (median)
out q["q3"]                # 75th percentile
out q["iqr"]               # interquartile range

# Full summary dict
summary = s.summary()
```

### Comparing two datasets

```ez
a = Statistics([1, 2, 3, 4, 5])
b = Statistics([2, 4, 6, 8, 10])

out a.covariance(b)        # covariance
out a.correlation(b)       # Pearson correlation coefficient (1.0 = perfect)
```

### Statistics Method Reference

| Method | Returns | Description |
|---|---|---|
| `.count()` | int | Number of data points |
| `.sum()` | float | Sum of all values |
| `.mean()` | float | Arithmetic mean |
| `.median()` | float | Middle value |
| `.mode()` | float | Most frequent value |
| `.min()` | float | Minimum value |
| `.max()` | float | Maximum value |
| `.range()` | float | max − min |
| `.variance()` | float | Population variance |
| `.stddev()` | float | Standard deviation |
| `.sem()` | float | Standard error of the mean |
| `.percentile(p)` | float | p-th percentile (0–100) |
| `.quartiles()` | dict | `{ q1, q2, q3, iqr }` |
| `.covariance(other)` | float | Covariance with another dataset |
| `.correlation(other)` | float | Pearson correlation coefficient |
| `.summary()` | dict | All key stats in one dict |

---

## Global Aliases

```ez
vec2 = Vector2        # shorthand constructor
vec3 = Vector3
mat2 = Matrix2x2
stats = |data| { give Statistics(data) }

# Example
v = vec2(1, 2)
s = stats([1, 2, 3, 4, 5])
```

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
