# eztest

> Production-ready unit testing framework for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**eztest** brings a full Jest/Mocha-style testing experience to EZ. It features test suites, lifecycle hooks, 15 assertion types, spies/stubs/mocks, colored terminal output, and a summary report — all in pure EZ.

---

## Installation

```
ez install test
```

---

## Quick Start

```ez
use "test"

describe("Math", task() {
    it("adds two numbers", task() {
        assertEqual(1 + 1, 2)
    })

    it("handles negatives", task() {
        assertEqual(-5 + 3, -2)
    })
})

run()
```

```
═══ EZ Test Framework ═══

Math
  ✓ adds two numbers
  ✓ handles negatives

═══════════════════════════════
  2 passed

  Duration: 1ms
═══════════════════════════════
```

---

## Defining Tests

### `describe` / `suite` — Group related tests

```ez
describe("User Model", task() {
    it("creates a user", task() {
        user = { "name": "Alice", "age": 30 }
        assertNotNull(user)
        assertEqual(user["name"], "Alice")
    })
})

# describe and suite are identical
suite("String Utils", task() {
    it("trims whitespace", task() {
        assertEqual(trim("  hello  "), "hello")
    })
})
```

### Nested Suites

```ez
describe("API", task() {
    describe("GET /users", task() {
        it("returns 200", task() {
            assertEqual(200, 200)
        })
    })

    describe("POST /users", task() {
        it("returns 201", task() {
            assertEqual(201, 201)
        })
    })
})
```

### `it` / `testCase` — Individual test cases

```ez
it("standalone test outside a suite", task() {
    assert(true)
})

testCase("alternative syntax", task() {
    assertEqual(1, 1)
})
```

### `skipe` — Skip a test

```ez
describe("Feature X", task() {
    it("works normally", task() {
        assert(true)
    })

    skipe("not implemented yet", task() {
        assert(false)   # never runs
    })
})
```

Skipped tests show as yellow in the output and are counted separately.

---

## Lifecycle Hooks

Hooks run automatically around your tests. All four are available per suite.

```ez
describe("Database", task() {
    beforeAll(task() {
        # Runs once before all tests in this suite
        db = connect("test.db")
    })

    afterAll(task() {
        # Runs once after all tests in this suite
        db.close()
    })

    beforeEach(task() {
        # Runs before every individual test
        db.clear()
        db.seed()
    })

    afterEach(task() {
        # Runs after every individual test
        db.rollback()
    })

    it("inserts a record", task() {
        db.insert({ "id": 1, "name": "Alice" })
        assertEqual(db.count(), 1)
    })

    it("deletes a record", task() {
        db.delete(1)
        assertEqual(db.count(), 0)
    })
})
```

---

## Assertions

All assertions throw a descriptive error message on failure. An optional final `message` argument overrides the default.

### Boolean

```ez
assert(value)                    # value is truthy
assertFalse(value)               # value is falsy
```

### Equality

```ez
assertEqual(actual, expected)           # actual == expected
assertNotEqual(actual, notExpected)     # actual != notExpected
assertDeepEqual(actual, expected)       # deep equality for arrays and dicts
```

### Null Checks

```ez
assertNull(value)       # value is nil
assertNotNull(value)    # value is not nil
```

### Type Check

```ez
assertType(value, "string")       # typeOf(value) == "string"
assertType(value, "number")
assertType(value, "array")
assertType(value, "dictionary")
```

### String / Array Membership

```ez
assertContains("hello world", "world")        # string contains substring
assertInArray([1, 2, 3], 2)                   # array contains value
```

### Numeric Comparisons

```ez
assertGreaterThan(10, 5)                      # 10 > 5
assertLessThan(3, 10)                         # 3 < 10
assertApproxEqual(3.14159, 3.14, 0.01)        # within tolerance
```

### Error Handling

```ez
# Assert a function throws
assertThrows(task() {
    throw "oops"
})

# Assert it throws a specific message
assertThrows(task() {
    throw "invalid input"
}, "invalid")

# Assert a function does NOT throw
assertDoesNotThrow(task() {
    x = 1 + 1
})
```

### Full Assertion Reference

| Assertion | Description |
|---|---|
| `assert(val)` | Passes if val is truthy |
| `assertFalse(val)` | Passes if val is falsy |
| `assertEqual(a, b)` | Passes if `a == b` |
| `assertNotEqual(a, b)` | Passes if `a != b` |
| `assertDeepEqual(a, b)` | Deep equality for arrays and dicts |
| `assertNull(val)` | Passes if val is `nil` |
| `assertNotNull(val)` | Passes if val is not `nil` |
| `assertType(val, type)` | Passes if `typeOf(val) == type` |
| `assertContains(str, sub)` | Passes if string contains substring |
| `assertInArray(arr, val)` | Passes if array contains value |
| `assertGreaterThan(val, n)` | Passes if `val > n` |
| `assertLessThan(val, n)` | Passes if `val < n` |
| `assertApproxEqual(a, b, tol)` | Passes if `abs(a - b) <= tol` (default: `0.0001`) |
| `assertThrows(fn, msg?)` | Passes if fn throws (optionally matching msg) |
| `assertDoesNotThrow(fn)` | Passes if fn does not throw |

---

## Spies, Stubs & Mocks

### Spy — Track calls to a real function

```ez
use "test"

called = false
original = task(x) {
    called = true
    give x * 2
}

s = Spy(original)
s.call(5)

assertEqual(s.callCount(), 1)
assert(s.calledWith(5))
assertEqual(s.getCall(0)["args"], 5)
```

### Stub — Replace a function with a fixed return value

```ez
double = stub(42)
result = double(99)   # ignores input, always returns 42
assertEqual(result, 42)
```

### Mock — Empty spy with no implementation

```ez
fn = mock()
fn("hello")
fn("world")

# Use the underlying Spy to inspect
s = Spy(fn)
# check s.callCount(), s.calledWith(), etc.
```

### Spy Methods

| Method | Description |
|---|---|
| `.call(args)` | Invoke the spy |
| `.callCount()` | Number of times called |
| `.getCall(index)` | Get call info at index (`{ args, timestamp }`) |
| `.calledWith(args)` | Returns `true` if ever called with these args |
| `.reset()` | Clear call history |
| `.returns(val)` | Set stub return value (disables call-through) |

---

## Running Tests

### `run()` — Run all tests and exit

```ez
run()
```

Exits with code `1` if any tests fail — ideal for CI pipelines.

### `runTests(options)` — Run and return results

```ez
results = runTests({})

out results["passed"]    # number of passing tests
out results["failed"]    # number of failing tests
out results["skipped"]   # number of skipped tests
out results["total"]     # total tests run
out results["duration"]  # milliseconds
out results["success"]   # true if zero failures
```

### Silent Mode — Suppress output

```ez
results = runTests({ "silent": true })
# No terminal output; use results dict programmatically
```

### `clear()` — Reset test state

```ez
clear()   # wipe all registered suites and reset counters
```

---

## Prefixed Global Aliases

If you prefer not to use `test.` or the bare function names, every function is also exported with a `t` prefix to avoid naming conflicts:

```ez
tDescribe, tSuite, tIt, tTestCase, tSkip
tBeforeEach, tAfterEach, tBeforeAll, tAfterAll
tAssert, tAssertEqual, tAssertDeepEqual, tAssertNotEqual
tAssertNull, tAssertNotNull, tAssertType
tAssertContains, tAssertInArray
tAssertGreaterThan, tAssertLessThan, tAssertApproxEqual
tAssertThrows, tAssertDoesNotThrow, tAssertFalse
tSpy, tStub, tMock
tRun, tRunTests, tClear
```

---

## Complete Example

```ez
use "test"

describe("String Utils", task() {
    describe("trim", task() {
        it("removes leading spaces", task() {
            assertEqual(trim("  hello"), "hello")
        })

        it("removes trailing spaces", task() {
            assertEqual(trim("hello  "), "hello")
        })

        skip("handles tabs (TODO)", task() {
            assertEqual(trim("\thello\t"), "hello")
        })
    })

    describe("split", task() {
        beforeEach(task() {
            # setup per-test state here
        })

        it("splits on delimiter", task() {
            parts = split("a,b,c", ",")
            assertDeepEqual(parts, ["a", "b", "c"])
        })

        it("returns single element when no match", task() {
            parts = split("hello", ",")
            assertEqual(len(parts), 1)
        })
    })
})

describe("Math Utils", task() {
    it("abs handles negatives", task() {
        assertEqual(abs(-5), 5)
    })

    it("approx equal for floats", task() {
        assertApproxEqual(0.1 + 0.2, 0.3, 0.0001)
    })

    it("throws on divide by zero", task() {
        assertThrows(task() {
            x = 1 / 0
        })
    })
})

run()
```

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
