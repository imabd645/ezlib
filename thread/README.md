# thread — Concurrency Library for EZ

> **Version:** 1.0  
> **Import:** `use "thread"`  
> **File:** `E:\ezlib\thread\main.ez`

---

## Overview

`thread` is EZ's official concurrency library, providing high-level primitives for writing parallel and asynchronous code. It wraps EZ's built-in `async` / `await` keywords with ergonomic combinators for common concurrent patterns.

The library provides:
- **`all(futures)`** — Await an array of futures
- **`parallelMap(items, mapper)`** — Process items in parallel
- **`TaskGroup`** — Collect and manage async task groups
- **`sleep(ms)`** — Non-blocking pause

---

## Quick Start

```ez
use "thread"

# Run two slow operations concurrently
task slow1() { wait(500)   give "Result 1" }
task slow2() { wait(300)   give "Result 2" }

f1 = async slow1()
f2 = async slow2()
results = thread.all([f1, f2])
out results   # → ["Result 1", "Result 2"]
# Total time: ~500ms (not 800ms)
```

---

## Global Variables Exported

After `use "thread"`, the following are available in the global scope:
- `all` — function
- `parallelMap` — function
- `createGroup` — function
- `sleep` — function
- `TaskGroup` — model

And via `thread.` prefix (imported as globals too).

---

## API Reference

### `all(futures)` → `array`

Awaits an array of futures sequentially and returns their results.

**Parameters:**
- `futures` — Array of future handles (values returned from `async fn()`).

**Returns:** Array of results in the same order as the input futures.

```ez
use "thread"

task fetchA() { wait(200)   give "Data A" }
task fetchB() { wait(100)   give "Data B" }
task fetchC() { wait(300)   give "Data C" }

# Spawn all concurrently
fa = async fetchA()
fb = async fetchB()
fc = async fetchC()

# Wait for all (concurrently running)
results = all([fa, fb, fc])
out results[0]  # → "Data A"
out results[1]  # → "Data B"
out results[2]  # → "Data C"
# Total time ~300ms (longest), not 600ms
```

---

### `parallelMap(items, mapper)` → `array`

Applies a mapper function to each item in an array concurrently, returning all results.

**Parameters:**
- `items` — Array of input values.
- `mapper` — A function (or lambda) to apply to each item.

**Returns:** Array of results in the same order as the input items.

This is the EZ equivalent of `Promise.all(items.map(fn))` in JavaScript.

```ez
use "thread"

# Double each number in parallel
numbers = [1, 2, 3, 4, 5]
doubled = parallelMap(numbers, |x| { give x * 2 })
out doubled   # → [2, 4, 6, 8, 10]
```

```ez
use "thread"

# Fetch multiple URLs in parallel (using ezhttp)
use "ezhttp"

urls = [
    "https://api.example.com/users",
    "https://api.example.com/products",
    "https://api.example.com/orders"
]

responses = parallelMap(urls, |url| {
    give http.get(url)
})

get r in responses {
    out r["status"]   # → 200
}
```

```ez
use "thread"

# Process a batch of files
use "ezfs"

files = fs.listDir("data")
results = parallelMap(files, |name| {
    content = fs.readFile("data\\" + name)
    give len(content)
})

out "File sizes: " + str(results)
```

---

### `createGroup()` → `TaskGroup`

Creates a new `TaskGroup` for managing multiple async operations together.

```ez
use "thread"

group = createGroup()
```

---

### `sleep(ms)`

Pauses execution for `ms` milliseconds without blocking the event loop.

Wraps the EZ built-in `wait(ms)`.

```ez
use "thread"

out "Starting..."
sleep(1000)   # Wait 1 second
out "Done after 1 second"
```

```ez
use "thread"

# Polling with sleep
task waitForReady() {
    attempts = 0
    while attempts < 10 {
        when checkIfReady() {
            give true
        }
        sleep(500)   # Wait 500ms between checks
        attempts = attempts + 1
    }
    give false
}
```

---

## Model: `TaskGroup`

A `TaskGroup` collects async tasks so they can be started and awaited together.

### `TaskGroup()`
Creates a new task group.

```ez
use "thread"

group = TaskGroup()
```

---

### `group.add(func, arg)` → `future`

Adds a new async task to the group. The task `func(arg)` is started immediately in the background.

**Parameters:**
- `func` — A callable (task, lambda).
- `arg` — The single argument to pass to the function.

**Returns:** The future handle for this specific task (for individual awaiting if needed).

```ez
use "thread"

task processUser(userId) {
    wait(100)   # simulate work
    give "Processed user " + str(userId)
}

group = TaskGroup()
group.add(processUser, 1)
group.add(processUser, 2)
group.add(processUser, 3)

results = group.wait()
out results   # → ["Processed user 1", "Processed user 2", "Processed user 3"]
```

---

### `group.wait()` → `array`

Awaits all tasks in the group and returns their results in order.

```ez
use "thread"

task compute(n) {
    wait(n * 100)   # different durations
    give n * n
}

group = createGroup()
group.add(compute, 3)
group.add(compute, 1)
group.add(compute, 2)

results = group.wait()
out results   # → [9, 1, 4] (in order of add, not completion)
```

---

## Patterns & Examples

### Pattern: Fan-Out

Spawn multiple operations concurrently and wait for all:

```ez
use "thread"

task sendNotification(userId) {
    # Simulate async email send
    wait(200)
    out "Notified user " + str(userId)
    give true
}

userIds = [101, 102, 103, 104, 105]
futures = []

get id in userIds {
    push(futures, async sendNotification(id))
}

results = all(futures)
out "All notified: " + str(len(results)) + " users"
```

---

### Pattern: Worker Pool via parallelMap

```ez
use "thread"

task processTask(task) {
    wait(task["delay"])
    give task["name"] + " completed"
}

tasks = [
    {"name": "task-a", "delay": 300},
    {"name": "task-b", "delay": 100},
    {"name": "task-c", "delay": 200}
]

results = parallelMap(tasks, processTask)

get r in results {
    out r
}
# → "task-a completed"
# → "task-b completed"
# → "task-c completed"
# Total time: ~300ms (parallel)
```

---

### Pattern: Sequential vs Parallel Comparison

```ez
use "thread"

task slowOp(label) {
    wait(500)
    give label + " done"
}

# Sequential: 1500ms total
# start = clock()
# r1 = slowOp("A")
# r2 = slowOp("B")
# r3 = slowOp("C")
# out clock() - start  # → ~1500

# Parallel: ~500ms total
start = clock()
results = parallelMap(["A", "B", "C"], |label| {
    give slowOp(label)
})
out clock() - start   # → ~500
out results
```

---

### Pattern: Timeout Race

```ez
use "thread"

task withTimeout(fn, timeoutMs) {
    result = nil
    done = false
    
    # Spawn the real task
    f = async fn()
    
    # Spawn a timeout sentinel
    timeoutF = async (|| {
        wait(timeoutMs)
        give "__timeout__"
    })()
    
    first = await f
    when first == "__timeout__" {
        throw "Operation timed out after " + str(timeoutMs) + "ms"
    }
    give first
}
```

---

### Pattern: Parallel File Processing

```ez
use "thread"
use "ezfs"

inputDir = "input"
outputDir = "output"

task processFile(filename) {
    content = fs.readFile(inputDir + "\\" + filename)
    # Transform: convert to uppercase
    result = upper(content)
    fs.writeFile(outputDir + "\\" + filename, result)
    give filename + " processed (" + str(len(content)) + " bytes)"
}

files = fs.listDir(inputDir)
results = parallelMap(files, processFile)

get r in results {
    out "✓ " + r
}
```

---

### Pattern: Async Init with TaskGroup

```ez
use "thread"
use "ezdb"
use "ezfs"

task initDatabase() {
    wait(100)  # simulate async DB connection
    db = Database(":memory:")
    give {"status": "ok", "db": db}
}

task loadConfig() {
    wait(50)   # simulate async config read
    give {"theme": "dark", "lang": "en"}
}

task checkNetwork() {
    wait(200)  # simulate async network ping
    give {"latency": 35, "connected": true}
}

group = createGroup()
group.add(initDatabase, nil)
group.add(loadConfig, nil)
group.add(checkNetwork, nil)

results = group.wait()
dbResult = results[0]
configResult = results[1]
netResult = results[2]

out "DB: " + dbResult["status"]
out "Theme: " + configResult["theme"]
out "Latency: " + str(netResult["latency"]) + "ms"
```

---

## Edge Cases & Important Notes

### `all()` is Sequential Awaiting
`all()` iterates the futures array and calls `await` on each one sequentially. The concurrent execution happens because you spawned all tasks with `async` BEFORE calling `all()`. The awaiting is sequential, but the tasks run in parallel.

```ez
# ✅ CORRECT — tasks start BEFORE all()
f1 = async task1()
f2 = async task2()
results = all([f1, f2])  # awaits both, which are already running

# ❌ WRONG — tasks start inside all(), which is sequential
# (This would be wrong if all() did async fn() internally per element)
```

### Single-Argument Limitation in `TaskGroup.add()`
`TaskGroup.add(func, arg)` only supports a **single argument**. For multi-argument functions, wrap them in a lambda:

```ez
use "thread"

task multiArg(a, b) { give a + b }

group = createGroup()

# ❌ This won't work correctly:
# group.add(multiArg, [1, 2])  # arg is the array, not spread

# ✅ Wrap in a lambda:
group.add(|args| { give multiArg(args[0], args[1]) }, [1, 2])

results = group.wait()
out results[0]  # → 3
```

### No Error Propagation from `all()`
If a future's task throws an error, `await` will propagate it. This will crash the `all()` call. Use `try/catch` around futures that might fail:

```ez
use "thread"

task riskyOp(x) {
    when x == 0 { throw "Cannot process zero!" }
    give x * 2
}

# Wrap each task with error handling
task safeOp(x) {
    try {
        give riskyOp(x)
    } catch e {
        give nil   # Return nil instead of throwing
    }
}

results = parallelMap([1, 0, 3], safeOp)
out results   # → [2, nil, 6]
```

### Thread Safety
EZ's async model is cooperative (not preemptive). Tasks yield at `wait()` / `await` points. Shared mutable state between concurrent tasks is generally safe unless tasks yield in the middle of a multi-step mutation.

---

## Full Example: Concurrent Web Scraper

```ez
use "thread"
use "ezhttp"
use "ezregex"

urls = [
    "https://httpbin.org/get",
    "https://httpbin.org/ip",
    "https://httpbin.org/user-agent"
]

task scrape(url) {
    response = http.get(url)
    when response["status"] == 200 {
        give {
            "url": url,
            "size": len(response["body"]),
            "ok": true
        }
    }
    give {"url": url, "ok": false}
}

out "Scraping " + str(len(urls)) + " pages concurrently..."
start = clock()

results = parallelMap(urls, scrape)

elapsed = clock() - start
out "Done in " + str(elapsed) + "ms"

get r in results {
    when r["ok"] {
        out "✓ " + r["url"] + " (" + str(r["size"]) + " bytes)"
    } other {
        out "✗ " + r["url"] + " failed"
    }
}
```

---

*Documentation generated from `E:\ezlib\thread\main.ez` — EZ Thread Library*
