# ezlib — The EZ Standard Library Registry

> The official central package registry for the [EZ Programming Language](https://github.com/imabd645/EZ-language).  
> Every package installed via `ez install` is sourced from this repository.

---

## What is this?

`ezlib` is the monorepo that hosts all official standard libraries for EZ. When you run `ez install <package>`, the EZ package manager downloads the corresponding folder from this repository, reads its `package.ez` manifest, and installs it to `C:/ezlib` on your machine — making it immediately available to any EZ script via `use`.

No separate registry server. No account required. GitHub is the registry.

---

## Installing a Package

```
ez install <package>
```

That's it. The EZ interpreter handles downloading, extracting, and resolving dependencies automatically.

**Examples:**

```
ez install math
ez install gui
ez install collections
ez install datetime
ez install database
ez install http
ez install orm
ez install ai
ez install fs
ez install os
ez install pdf
```

After installation, use any package in your script:

```ez
use "math"
use "gui"
use "collections"
```

---

## Available Packages

| Package | Install Command | Description |
|---------|----------------|-------------|
| **ezmath** | `ez install math` | Vector math (`Vector2`, `Vector3`), statistics, trigonometry, lerp, clamp |
| **ezgui** | `ez install gui` | Native Win32 GUI — windows, buttons, inputs, dropdowns, panels, tabs, dark mode |
| **ezcollections** | `ez install collections` | `Set`, `Map`, `Queue`, `Stack`, `List`, `Counter` data structures |
| **ezdatetime** | `ez install datetime` | Timestamps, date formatting, arithmetic, leap year, timezone offset |
| **ezdb** | `ez install database` | SQLite wrapper — parameterized queries, CRUD, transactions |
| **ezhttp** | `ez install http` | HTTP client — GET, POST, custom headers, response handling |
| **ezorm** | `ez install orm` | Object-relational mapper built on top of ezdb |
| **ezai** | `ez install ai` | AI integration utilities for building intelligent EZ applications |
| **ezfs** | `ez install fs` | File system — read, write, copy, move, list directories |
| **ezos** | `ez install os` | OS interaction — environment variables, processes, system info |
| **ezpdf** | `ez install pdf` | PDF generation and manipulation |

---

## Quick Start Examples

### Math & Statistics

```ez
use "math"

// Vector operations
pos = Vector3(10, 5, 0)
dir = Vector3(1, 1, 0).normalize()
out str(pos.dot(dir))

// Statistics
scores = [85, 92, 78, 95, 88]
stats = Statistics(scores)
out "Average: " + str(stats.mean())
out "Std Dev: " + str(stats.stddev())

// Trigonometry (degrees)
out "sin(45°) = " + str(sind(45))
```

### GUI Application

```ez
use "gui"

gui.setTheme("dark")
win = gui.window("My App", 800, 600)

// Screen switching pattern
mainView = win.panel(0, 0, 800, 600)
settingsView = win.panel(0, 0, 800, 600).hide()

mainView.label("Welcome to EZ!", 20, 20, 300, 40)
mainView.button("Go to Settings", 20, 80, 160, 36, || {
    mainView.hide()
    settingsView.show()
})

settingsView.label("Settings", 20, 20, 200, 40)
settingsView.button("Back", 20, 80, 100, 36, || {
    settingsView.hide()
    mainView.show()
})

win.run()
```

### Collections

```ez
use "collections"

// Set — unique elements
s = Set().add(1).add(2).add(1)
out str(s.has(2))   // true
out str(s.size())   // 2

// Map — key-value pairs
m = Map().set("name", "EZ").set("version", "1.0")
out m.get("name")   // EZ

// Queue — FIFO
q = Queue().enqueue("first").enqueue("second")
out q.dequeue()     // first

// Stack — LIFO
st = Stack().push(10).push(20)
out str(st.pop())   // 20
```

### DateTime

```ez
use "datetime"

// Current date and time
out "Today: " + dateNow()
out "Now:   " + format(now())

// Create a specific timestamp
birthday = makeTimestamp(1995, 6, 15, 0, 0, 0)
out "Birthday: " + dateStr(birthday)

// Date arithmetic
nextWeek = addTime(now(), 7, 0, 0, 0)
out "Next week: " + format(nextWeek)

// Difference
out str(diffDays(nextWeek, now())) + " days away"
```

### Database

```ez
use "database"

db = openDatabase("app.db")
db.createTable("users", ["id INTEGER PRIMARY KEY", "name TEXT", "age INTEGER"])

// Insert with dictionary
id = db.insert("users", {name: "Alice", age: 30})
out "Inserted ID: " + str(id)

// Parameterized query
results = db.query("SELECT * FROM users WHERE age > ?", [25])
get user in results {
    out user.name + " — " + str(user.age)
}

db.disconnect()
```

### HTTP

```ez
use "http"

// Simple GET
response = http.get("https://api.example.com/data")
out response.body

// POST with JSON
response = http.post("https://api.example.com/users", {
    name: "Alice",
    email: "alice@example.com"
})
out str(response.status)
```

---

## How the Package Manager Works

When you run `ez install <name>`, the EZ interpreter:

1. Constructs the download URL from this repository
2. Downloads the package folder as a `.zip` archive via `curl`
3. Extracts it to `C:/ezlib/<name>/`
4. Reads `package.ez` to find the entry point and any dependencies
5. Recursively installs dependencies
6. Saves metadata to `C:/ezlib/packages.json`

At runtime, `use "name"` resolves through this chain:

```
local file → C:/ezlib/name → package.ez main field
           → C:/ezlib/name.ez → C:/ezlib/name/main.ez
```

### Package Manifest Format

Every package in this registry includes a `package.ez` manifest:

```json
{
  "name": "math",
  "version": "2.0.0",
  "description": "Math utilities for EZ",
  "author": "imabd645",
  "main": "math.ez",
  "dependencies": {}
}
```

---

## Creating Your Own Package

You can scaffold a new package with:

```
ez init mypackage
```

This creates:

```
mypackage/
  package.ez    ← manifest
  main.ez       ← entry point
```

A minimal `package.ez`:

```json
{
  "name": "mypackage",
  "version": "1.0.0",
  "description": "My EZ package",
  "main": "main.ez"
}
```

To make it installable, push it to GitHub as `imabd645/ezlib/ez<name>` and anyone can install it with `ez install <name>`.

---

## Listing Installed Packages

```
ez list
```

Output:

```
Installed packages:
 - math (2.0.0)
 - gui (6.0.0)
 - collections (1.0.0)
 - datetime (2.0.0)
```

---

## Repository Structure

```
ezlib/
├── ezai/           ← AI integration utilities
├── ezcollections/  ← Data structures (Set, Map, Queue, Stack, List)
├── ezdatetime/     ← Date and time manipulation
├── ezdb/           ← SQLite database wrapper
├── ezfs/           ← File system utilities
├── ezgui/          ← Native Win32 GUI framework
├── ezhttp/         ← HTTP client
├── ezmath/         ← Math, vectors, statistics, trigonometry
├── ezorm/          ← Object-relational mapper
├── ezos/           ← OS and system utilities
└── ezpdf/          ← PDF generation
├── ezserve/        ← A Web server in EZ
├── eztest/         ← A testing Framework for EZ
└── ezregex/        ← Regex libarary for EZ
```

---

## Requirements

- [EZ Language Interpreter](https://github.com/imabd645/EZ-language) — Windows (x64)
- `curl` available in PATH (used by the package manager for downloads)
- `tar` available in PATH (used for extraction — included in Windows 10+)

---

## License

All packages in this registry are licensed under the MIT License.

---

## Links

- [EZ Language Interpreter](https://github.com/imabd645/EZ-language) — The core interpreter

