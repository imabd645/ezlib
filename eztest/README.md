# eztest — Unit Testing Framework for EZ

> **Version:** 1.0.0  
> **Import:** `use "eztest"`  
> **File:** `E:\ezlib\eztest\main.ez`

---

## Overview

`eztest` is a production-ready unit testing framework for EZ, providing:

- **Test suites** with `describe()` / `it()` blocks
- **Lifecycle hooks** (`beforeEach`, `afterEach`, `beforeAll`, `afterAll`)
- **Rich assertions** (equality, type, deep, range, throw, async)
- **Mock / Spy / Stub** system
- **Colored terminal output** with pass/fail/skip reporting
- **Programmatic results** for CI/CD integration
- **Global shorthand aliases** for all functions

---

## Quick Start

```ez
use "eztest"

describe("Math Tests", || {
    it("adds numbers correctly", || {
        assertEqual(1 + 1, 2)
    })
    
    it("multiplies correctly", || {
        assertEqual(3 * 4, 12)
    })
})

run()
```

**Output:**
```
═══ EZ Test Framework ═══

Math Tests
  ✓ adds numbers correctly
  ✓ multiplies correctly

═══════════════════════════════
  2 passed

  Duration: 1ms
═══════════════════════════════
```

---

## Test Structure

### `describe(suiteName, fn)`
Defines a named test suite. Suites can be nested.

### `suite(name, fn)`
Alias for `describe`.

### `it(testName, fn)`
Defines a test case inside a `describe` block.

### `testCase(name, fn)`
Alias for `it`.

### `skipe(name, fn)`
Defines a **skipped** test case (it won't run, shown as skipped in output).

```ez
use "eztest"

describe("String Tests", || {
    it("length works", || {
        assertEqual(len("hello"), 5)
    })
    
    it("indexOf works", || {
        assertEqual(indexOf("hello world", "world"), 6)
    })
    
    skipe("concat works", || {
        # TODO: implement this test
        assertEqual("a" + "b", "ab")
    })
})

run()
```

---

## Lifecycle Hooks

### `beforeEach(fn)`
Runs before each `it()` in the current suite.

### `afterEach(fn)`
Runs after each `it()` in the current suite.

### `beforeAll(fn)`
Runs once before all tests in the suite.

### `afterAll(fn)`
Runs once after all tests in the suite.

```ez
use "eztest"
use "ezdb"

db = nil

describe("Database Tests", || {
    beforeAll(|| {
        db = Database(":memory:")
        db.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)", nil)
    })
    
    beforeEach(|| {
        # Clean state before each test
        db.execute("DELETE FROM users", nil)
    })
    
    afterAll(|| {
        db.close()
    })
    
    it("inserts a user", || {
        db.execute("INSERT INTO users (name) VALUES (?)", ["Alice"])
        count = db.fetchScalar("SELECT COUNT(*) FROM users", nil)
        assertEqual(count, 1)
    })
    
    it("finds no users initially", || {
        count = db.fetchScalar("SELECT COUNT(*) FROM users", nil)
        assertEqual(count, 0)
    })
})

run()
```

---

## Assertions

### `assert(value, message)` → `boolean`
Asserts that `value` is truthy. Throws `message` if not.

```ez
use "eztest"

assert(true)                         # passes
assert(1 == 1)                        # passes
assert(len("hello") == 5, "len failed")  # passes
assert(false, "Expected truthy")     # throws
```

---

### `assertFalse(value, message)`
Asserts that `value` is falsy.

```ez
assertFalse(false)           # passes
assertFalse(0)               # passes
assertFalse(true, "was true")  # throws
```

---

### `assertEqual(actual, expected, message)`
Asserts `actual == expected`.

```ez
assertEqual(2 + 2, 4)
assertEqual("hello", "hello")
assertEqual(nil, nil)
assertEqual(1, 2)   # throws: "Expected 2, got: 1"
```

---

### `assertNotEqual(actual, notExpected, message)`
Asserts `actual != notExpected`.

```ez
assertNotEqual("hello", "world")   # passes
assertNotEqual(42, 42)             # throws
```

---

### `assertDeepEqual(actual, expected, message)`
Asserts deep structural equality for arrays and dictionaries.

```ez
assertDeepEqual([1, 2, 3], [1, 2, 3])   # passes
assertDeepEqual({"a": 1, "b": 2}, {"a": 1, "b": 2})  # passes
assertDeepEqual([1, 2], [1, 3])  # throws
assertDeepEqual({"a": 1}, {"a": 2})  # throws
```

---

### `assertNull(value, message)`
Asserts that `value` is `nil`.

```ez
assertNull(nil)          # passes
assertNull(0)            # throws: "Expected null, got: 0"
```

---

### `assertNotNull(value, message)`
Asserts that `value` is NOT `nil`.

```ez
assertNotNull("hello")   # passes
assertNotNull(nil)        # throws: "Expected non-null value"
```

---

### `assertType(value, expectedType, message)`
Asserts that `typeOf(value) == expectedType`.

Valid type strings: `"string"`, `"number"`, `"boolean"`, `"nil"`, `"array"`, `"dictionary"`, `"function"`, `"model"`.

```ez
assertType("hello", "string")   # passes
assertType(42, "number")         # passes
assertType([], "array")          # passes
assertType({}, "dictionary")     # passes
assertType(42, "string")         # throws: "Expected type 'string', got: 'number'"
```

---

### `assertContains(haystack, needle, message)`
Asserts that string `haystack` contains string `needle`.

```ez
assertContains("Hello World", "World")   # passes
assertContains("Hello World", "xyz")     # throws
```

---

### `assertInArray(array, value, message)`
Asserts that `value` exists in the array.

```ez
assertInArray([1, 2, 3], 2)    # passes
assertInArray(["a", "b"], "c") # throws
```

---

### `assertThrows(fn, expectedMessage, message)`
Asserts that calling `fn()` throws an error. Optionally checks that the error message contains `expectedMessage`.

```ez
assertThrows(|| { throw "Something went wrong" })  # passes (threw)
assertThrows(|| { throw "Connection failed" }, "Connection")  # passes
assertThrows(|| { x = 1 + 1 }, nil, "should have thrown")   # throws (did not throw)
assertThrows(|| { throw "abc" }, "xyz")  # throws (message mismatch)
```

---

### `assertDoesNotThrow(fn, message)`
Asserts that calling `fn()` does NOT throw.

```ez
assertDoesNotThrow(|| { x = 1 + 1 })   # passes
assertDoesNotThrow(|| { throw "oops" })  # throws: "Expected no error, but threw: oops"
```

---

### `assertApproxEqual(actual, expected, tolerance, message)`
Asserts that `|actual - expected| <= tolerance`. Default tolerance: `0.0001`.

```ez
assertApproxEqual(3.14159, 3.14, 0.01)      # passes
assertApproxEqual(0.1 + 0.2, 0.3, 0.0001)  # passes (floating point)
assertApproxEqual(1.0, 2.0, 0.5)            # throws
```

---

### `assertGreaterThan(value, threshold, message)`
Asserts `value > threshold`.

```ez
assertGreaterThan(5, 3)    # passes
assertGreaterThan(3, 5)    # throws
assertGreaterThan(5, 5)    # throws (not strictly greater)
```

---

### `assertLessThan(value, threshold, message)`
Asserts `value < threshold`.

```ez
assertLessThan(3, 5)    # passes
assertLessThan(5, 3)    # throws
```

---

## Mock / Spy / Stub System

### `spy(targetFn)`
Creates a function that records all calls to it.

```ez
use "eztest"

callLog = []
originalFn = |x| { push(callLog, x)   give x * 2 }

spyFn = spy(originalFn)
spyFn(5)
spyFn(10)

# Note: spy() returns a wrapper lambda, not a Spy model directly
# For full Spy functionality, use Spy() directly:
s = Spy(originalFn)
s.call(5)
s.call(10)
out s.callCount()          # → 2
out s.getCall(0)["args"]   # → 5
out s.calledWith(10)       # → true
s.reset()
out s.callCount()          # → 0
```

---

### `stub(returnValue)`
Creates a function that always returns the given value, recording calls.

```ez
use "eztest"

fetchUser = stub({"id": 1, "name": "Alice"})
result = fetchUser(42)
out result["name"]   # → "Alice"
```

---

### `mock()`
Creates a no-op function that records calls.

```ez
use "eztest"

logger = mock()
logger("info message")
logger("warn message")
# Calls are recorded but nothing happens
```

---

## Running Tests

### `runTests(options)` → `dictionary`

Runs all registered tests and suites. Returns a summary dictionary.

**Options:**
- `{"silent": true}` — Suppress all output (useful for programmatic use).

**Returns:**
```
{
  "passed": number,
  "failed": number,
  "skipped": number,
  "total": number,
  "duration": number,   # milliseconds
  "success": boolean
}
```

```ez
use "eztest"

describe("Suite A", || {
    it("test 1", || { assertEqual(1, 1) })
    it("test 2", || { assertEqual(2, 2) })
})

results = runTests({})
out results["passed"]   # → 2
out results["failed"]   # → 0
out results["success"]  # → true
```

---

### `run()` → `dictionary`
Runs tests and exits the process with code `1` if any tests failed.

```ez
use "eztest"

describe("Basics", || {
    it("works", || { assert(true) })
})

run()   # exits with 0 (success)
```

---

### `clear()`
Clears all registered tests for reloading.

---

## `test` Namespace Object

Everything is also available under the `test` dictionary:

```ez
use "eztest"

test.describe("My Tests", || {
    test.it("works", || {
        test.assertEqual(1 + 1, 2)
    })
})

test.run()
```

---

## Global Shorthand Aliases

| Global Alias | Maps To |
|---|---|
| `tAssert` | `test.assert` |
| `tAssertFalse` | `test.assertFalse` |
| `tAssertEqual` | `test.assertEqual` |
| `tAssertDeepEqual` | `test.assertDeepEqual` |
| `tAssertNotEqual` | `test.assertNotEqual` |
| `tAssertNull` | `test.assertNull` |
| `tAssertNotNull` | `test.assertNotNull` |
| `tAssertType` | `test.assertType` |
| `tAssertContains` | `test.assertContains` |
| `tAssertInArray` | `test.assertInArray` |
| `tAssertThrows` | `test.assertThrows` |
| `tAssertDoesNotThrow` | `test.assertDoesNotThrow` |
| `tAssertApproxEqual` | `test.assertApproxEqual` |
| `tAssertGreaterThan` | `test.assertGreaterThan` |
| `tAssertLessThan` | `test.assertLessThan` |
| `tDescribe` | `test.describe` |
| `tSuite` | `test.suite` |
| `tIt` | `test.it` |
| `tTestCase` | `test.test` |
| `tSkip` | `test.skip` |
| `tBeforeEach` | `test.beforeEach` |
| `tAfterEach` | `test.afterEach` |
| `tBeforeAll` | `test.beforeAll` |
| `tAfterAll` | `test.afterAll` |
| `tRun` | `test.run` |
| `tRunTests` | `test.runTests` |
| `tClear` | `test.clear` |
| `tSpy` | `test.spy` |
| `tStub` | `test.stub` |
| `tMock` | `test.mock` |

---

## Edge Cases & Important Notes

### Assertion Error Messages
All assertions accept an optional `message` parameter. If not provided, a default message is generated using `__formatValue()` which shows the actual and expected values.

### Error Handling in Assertions
Assertions use `throw` to signal failures. The test runner wraps each `it()` in a `try/catch`. Any uncaught error (not just assertion errors) counts as a test failure.

### Deep Equality Performance
`assertDeepEqual()` recursively walks arrays and dictionaries. For very large structures, this may be slow.

### Nested `describe()` Blocks
Suites can be nested. Inner suites inherit the current suite context but have their own `beforeEach`/`afterEach` hooks. Parent hooks do NOT automatically run for child suites.

### `beforeAll` Failure
If `beforeAll()` throws, the entire suite is skipped and an error is printed. Individual tests are not run.

### `afterEach` Errors
If `afterEach()` throws and the test itself passed, the test is marked as **failed** with the `afterEach` error message.

### Silent Mode
`runTests({"silent": true})` suppresses all `out` calls in the framework (but not in your test functions themselves).

---

## Full Example: Testing a Calculator Module

```ez
use "eztest"

# Calculator under test
task add(a, b) { give a + b }
task subtract(a, b) { give a - b }
task multiply(a, b) { give a * b }
task divide(a, b) {
    when b == 0 { throw "Division by zero" }
    give a / b
}

describe("Calculator", || {
    describe("Addition", || {
        it("adds positive numbers", || { assertEqual(add(2, 3), 5) })
        it("adds negative numbers", || { assertEqual(add(-2, -3), -5) })
        it("adds zero", || { assertEqual(add(5, 0), 5) })
    })
    
    describe("Division", || {
        it("divides correctly", || {
            assertApproxEqual(divide(10, 3), 3.333, 0.001)
        })
        
        it("throws on division by zero", || {
            assertThrows(|| { divide(5, 0) }, "Division by zero")
        })
        
        it("handles negative divisor", || {
            assertEqual(divide(-10, 2), -5)
        })
    })
    
    describe("Multiplication", || {
        it("multiplies", || { assertEqual(multiply(3, 4), 12) })
        it("zero product", || { assertEqual(multiply(0, 999), 0) })
    })
})

results = runTests({})
when not results["success"] {
    out "Some tests failed!"
}
```

---

## Full Example: CI-Friendly Test Runner

```ez
use "eztest"

# Run silently and check results
describe("Core", || {
    it("string operations work", || {
        assertEqual(len("hello"), 5)
        assertContains("hello world", "world")
    })
    
    it("array operations work", || {
        arr = [1, 2, 3]
        assertDeepEqual(arr, [1, 2, 3])
        assertInArray(arr, 2)
    })
    
    it("arithmetic is correct", || {
        assertApproxEqual(3.14 * 2, 6.28, 0.01)
        assertGreaterThan(10, 5)
        assertLessThan(5, 10)
    })
})

# Silent run for CI
results = runTests({"silent": true})

out "Tests: " + str(results["total"])
out "Passed: " + str(results["passed"])
out "Failed: " + str(results["failed"])
out "Duration: " + str(results["duration"]) + "ms"

when not results["success"] {
    out "FAILED"
    exit(1)
}

out "PASSED"
```

---

*Documentation generated from `E:\ezlib\eztest\main.ez` — EZ Test Framework v1.0.0*
