# decimal — exact decimal and money arithmetic for EZ

> **Import:** `use "decimal"`
> **Install:** `ez install decimal`

Floats cannot represent tenths. Every price, total and tax computed with them
is a little bit wrong, and it prints correctly right up until a total fails to
reconcile.

```ez
# Plain EZ numbers
out 0.1 + 0.2 == 0.3          # false

# decimal
use "decimal"
out Decimal("0.1").plus("0.2").equals("0.3")    # true
```

A `Decimal` is an integer count of units plus a scale, so every value it can
hold, it holds **exactly** — no drift, at any size.

---

## Quick Start

```ez
use "decimal"

price = Decimal("19.99")
qty   = 3

subtotal = price.times(qty)                 # 59.97, exactly
tax      = subtotal.percent(8.5)            # 5.10
total    = subtotal.plus(tax)

out total.currency("$", 2)                  # $65.07
```

## Why not just round at the end?

Because the error compounds before you get there:

```ez
total = 0
repeat i = 1 to 10 { total = total + 0.1 }
out total == 1.0                            # false — plain floats
```

Ten additions is enough to break equality. A shopping cart, an invoice with
line items, or a ledger does far more than ten.

## API

### Creating

```ez
Decimal("19.99")      # from a string — always prefer this for money
Decimal(42)           # from an integer
Decimal(other)        # copy of another Decimal
Decimal(nil)          # zero
dec("1.5")            # shorthand
money("1.005")        # rounded to 2 places
```

Underscores and commas are stripped, so `"1_000.25"` and `"1,000.25"` both
parse — a figure pasted from a spreadsheet works.

> **Prefer strings over floats.** `Decimal(0.1)` goes through the float's
> decimal spelling, and the float is already not a tenth by the time it
> arrives. `Decimal("0.1")` is exact.

Anything unparseable **throws** rather than becoming zero — a silent zero in
a price field is worse than a stopped program.

### Arithmetic

| Call | Notes |
| --- | --- |
| `a.plus(b)` / `a.minus(b)` | Scales align first, so nothing is lost |
| `a.times(b)` | Scales add: `1.5 × 1.5` is exact at scale 4 |
| `a.dividedBy(b)` | Rounds to a sensible scale |
| `a.dividedByAt(b, scale)` | Rounds to exactly `scale` places |
| `a.modulo(b)` | Remainder, keeping the dividend's sign |
| `a.negated()` / `a.absolute()` | |
| `a.percent(rate)` | `200.percent(15)` → `30.00` |
| `decSum(list)` / `decMax(list)` / `decMin(list)` | |

Any argument may be a string, integer or Decimal — `price.plus("0.50")` works.

**Division takes a scale** because it cannot be exact in general: one third has
no finite decimal form. Division by zero throws.

### Comparing

| Call | Result |
| --- | --- |
| `a.compare(b)` | `-1`, `0`, `1` |
| `a.equals(b)` | Scale-insensitive: `1.5` equals `1.50` |
| `a.isLess` / `isGreater` / `atMost` / `atLeast` | |
| `a.isZero` / `isNegative` / `isPositive` / `sign()` | |

### Rounding

```ez
d.roundTo(2)                  # using the current mode
d.roundWith(2, "half-even")   # one-off mode
d.floorTo(0)  d.ceilTo(0)  d.truncateTo(0)
d.rounding("half-even")       # set the mode; returns the Decimal
```

| Mode | Behaviour |
| --- | --- |
| `half-up` *(default)* | `.5` rounds away from zero — what people expect of money |
| `half-even` | `.5` rounds to the even neighbour — banker's rounding, no upward drift over many operations |
| `half-down` | `.5` rounds toward zero |
| `up` / `down` | Always away from / toward zero |
| `ceil` / `floor` | Always toward +∞ / −∞ |

Use `half-even` for anything that rounds repeatedly — interest, allocations,
long ledgers — because `half-up` biases every tie upward.

### Formatting

```ez
d.toString()                       # "1234.50" — always exactly `scale` places
d.format(2)                        # "1,234.50"
d.formatWith(2, ".", ",")          # "1.234,50" — European
d.currency("$", 2)                 # "$1,234.50"  (negatives: "-$5.00")
d.toInteger()                      # exact, or throws if there are cents
d.toFloat()                        # lossy — output only, never arithmetic
```

`toString` keeps trailing zeros, so `1.50` does not print as `1.5`.

## Splitting money without losing cents

Dividing money between people is where naive arithmetic visibly fails: 0.10
split three ways is 0.03 each, and three lots of 0.03 is 0.09. A cent
vanishes.

```ez
shares = Decimal("0.10").split(3)
# 0.04, 0.03, 0.03   — and they sum to exactly 0.10
```

`split(n)` hands the leftover units to the earliest shares, so the total
always reconciles. `allocate(weights)` does the same proportionally:

```ez
Decimal("100.00").allocate([50, 30, 20])    # 50.00, 30.00, 20.00
Decimal("0.05").allocate([3, 7])            # sums to exactly 0.05
```

Both work for negative amounts (refunds) too.

## Limits

- **Scale is capped at 18 decimal places**, and values are 64-bit integers
  internally. At 2 places that is roughly ±92 trillion — ample for money,
  but `times()` adds scales, so chained multiplications of very large,
  very precise numbers can overflow. Round between steps if you are near
  that range.
- **No exponent form.** `"1e6"` is rejected; write `"1000000"`.
- **Not a general numeric tower** — no square roots, logs or trigonometry.
  Use `math` for those, and keep `decimal` for values that must balance.

## Testing

```
ez test.ez
```

85 tests covering parsing, every arithmetic and rounding mode, comparison
across scales, formatting, remainder-safe splitting, and exactness at large
magnitudes.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Decimal` model and money helpers |
| `arith.ez` | Exact integer division, powers of ten, rounding modes |
| `parse.ez` | Text to units, and units back to formatted text |

## License

MIT
