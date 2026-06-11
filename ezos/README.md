# ezos — Operating System Utilities Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezos"`  
> **File:** `E:\ezlib\ezos\main.ez`  
> **Requires:** `Kernel32.dll`, `User32.dll`, `msvcrt.dll`, `shell32.dll`

---

## Overview

`ezos` is a comprehensive Windows system utilities library for EZ, exposing native OS APIs through FFI. After import, a ready-to-use `os` instance is available with sub-objects for:

- **`os.env`** — Environment variable reading, writing, and `.env` file loading
- **`os.path`** — Path manipulation utilities
- **`os.process`** — Shell command execution and process management
- **`os.clipboard`** — System clipboard read/write
- **`os.shell`** — OS shell integration (open files/URLs)
- **`os`** — System metrics (platform, uptime, memory, CWD)

---

## Quick Start

```ez
use "ezos"

out os.platform()       # → "windows"
out os.cwd()            # → "C:\Users\HP\Projects"
out os.uptime()         # → seconds since boot

out os.env.get("PATH")  # → PATH env variable string

result = os.process.exec("dir /b")
out result              # → directory listing
```

---

## `os` — System Information

### `os.platform()` → `string`
Always returns `"windows"`. `ezos` is Windows-only.

### `os.uptime()` → `number`
Returns the system uptime in seconds (floating point precision).

Uses `GetTickCount64` (milliseconds since boot) divided by 1000.

```ez
use "ezos"

secs = os.uptime()
mins = floor(secs / 60)
hours = floor(mins / 60)
out "System uptime: " + str(hours) + "h " + str(mins % 60) + "m"
```

### `os.totalMemory()` → `number`
Returns the total physical RAM in bytes.

```ez
use "ezos"

total = os.totalMemory()
out str(total / 1073741824) + " GB total RAM"
```

### `os.freeMemory()` → `number`
Returns the available physical RAM in bytes.

```ez
use "ezos"

free = os.freeMemory()
used = os.totalMemory() - free
pct = floor(used * 100 / os.totalMemory())
out "Memory usage: " + str(pct) + "%"
```

### `os.cwd()` → `string`
Returns the current working directory.

```ez
use "ezos"

out os.cwd()   # → "C:\Users\HP\Projects\myapp"
```

### `os.chdir(path)` → `boolean`
Changes the current working directory. Returns `true` on success.

```ez
use "ezos"

ok = os.chdir("C:\\Windows")
out ok             # → true
out os.cwd()       # → "C:\Windows"

ok = os.chdir("C:\\nonexistent")
out ok             # → false
```

---

## `os.env` — Environment Variables

### `os.env.get(key)` → `string | nil`
Gets an environment variable value. Returns `nil` if not set.

```ez
use "ezos"

out os.env.get("PATH")         # → system PATH string
out os.env.get("USERNAME")     # → "HP" or current Windows user
out os.env.get("TEMP")         # → temp directory path
out os.env.get("MY_APP_KEY")   # → nil if not set
```

### `os.env.set(key, value)`
Sets an environment variable for the current process and its children.

```ez
use "ezos"

os.env.set("MY_APP_ENV", "production")
os.env.set("DB_HOST", "localhost")
os.env.set("PORT", "8080")

out os.env.get("MY_APP_ENV")   # → "production"
```

### `os.env.load(path)` → `boolean`
Loads environment variables from a `.env` file into the process environment.

**Parameters:**
- `path` — File path (default: `".env"` if `nil` or empty).

**Format:** Each line in the `.env` file should be `KEY=VALUE`. Lines starting with `#` are ignored.

**Returns:** `true` if the file was loaded, `false` if not found.

```ez
use "ezos"

# .env file contents:
# DB_HOST=localhost
# DB_PORT=5432
# DB_PASS=secret
# # This is a comment

ok = os.env.load(".env")
when ok {
    out os.env.get("DB_HOST")   # → "localhost"
    out os.env.get("DB_PORT")   # → "5432"
}
```

**Multi-value handling:** If a value contains `=`, only the first `=` is used as separator; everything after is the value including subsequent `=` signs.

---

## `os.path` — Path Utilities

### `os.path.join(p1, p2)` → `string`
Joins two path segments with a backslash.

```ez
use "ezos"

out os.path.join("C:\\Users\\HP", "Documents")  # → "C:\Users\HP\Documents"
out os.path.join("logs", "app.log")             # → "logs\app.log"
```

### `os.path.isAbsolute(p)` → `boolean`
Returns `true` if the path starts with a drive letter (e.g. `C:`).

```ez
use "ezos"

out os.path.isAbsolute("C:\\Windows")    # → true
out os.path.isAbsolute("relative\\path") # → false
out os.path.isAbsolute("D:\\Data")       # → true
```

---

## `os.process` — Process & Shell Execution

### `os.process.exec(cmd)` → `string`
Executes a shell command and returns its **stdout** output as a string.

Uses `_popen` / `fgets` to capture output line by line.

```ez
use "ezos"

# Run any shell command
out os.process.exec("echo Hello World")    # → "Hello World\n"
out os.process.exec("whoami")              # → "DESKTOP-XYZ\HP\n"
out os.process.exec("dir /b C:\\Windows")  # → directory listing

# Run Python
out os.process.exec("python -c \"print(2+2)\"")  # → "4\n"

# Run an EZ script
out os.process.exec("ez myutil.ez")
```

**Note:** Does NOT capture stderr. Blocking — waits for command to complete.

### `os.process.system(cmd)` → `number`
Executes a shell command without capturing output. Returns the exit code.

```ez
use "ezos"

code = os.process.system("mkdir output 2>nul")
out code   # → 0 on success

os.process.system("start notepad.exe")  # Opens Notepad (non-blocking)
```

### `os.process.exit(code)`
Terminates the current EZ process with the given exit code.

```ez
use "ezos"

out "About to exit with code 1"
os.process.exit(1)
out "This line is never reached"
```

---

## `os.clipboard` — System Clipboard

### `os.clipboard.read()` → `string`
Reads text from the Windows clipboard.

```ez
use "ezos"

text = os.clipboard.read()
out "Clipboard contents: " + text
```

### `os.clipboard.write(text)` → `boolean`
Writes text to the Windows clipboard. Returns `true` on success.

```ez
use "ezos"

os.clipboard.write("Hello, Clipboard!")
out os.clipboard.read()   # → "Hello, Clipboard!"
```

### `os.clipboard.clear()` → `boolean`
Clears the clipboard contents (writes empty string).

```ez
use "ezos"

os.clipboard.write("some text")
os.clipboard.clear()
out os.clipboard.read()   # → ""
```

---

## `os.shell` — Shell Integration

### `os.shell.open(path)` → `number`
Opens a file, folder, or URL using the default Windows application.

Uses `ShellExecuteA` with verb `"open"`.

```ez
use "ezos"

# Open a file in its default app
os.shell.open("C:\\Users\\HP\\Documents\\report.pdf")

# Open a folder in Explorer
os.shell.open("C:\\Users\\HP\\Downloads")

# Open a URL in default browser
os.shell.open("https://example.com")

# Open a .ez file
os.shell.open("myscript.ez")
```

---

## Edge Cases & Important Notes

### Windows-Only
All features use Windows DLLs (`Kernel32.dll`, `User32.dll`, `msvcrt.dll`, `shell32.dll`). This library is **not portable** to Linux/macOS.

### `os.env.load()` Strips `\r`
The `.env` parser removes carriage return characters from values to handle Windows-style `\r\n` line endings.

### `os.env.load()` Values with Multiple `=`
Values containing `=` are handled correctly:
```
# .env
JWT_SECRET=a=b=c
```
→ `os.env.get("JWT_SECRET")` returns `"a=b=c"`.

### `os.process.exec()` Blocking
`exec()` blocks until the command finishes. For long-running processes, use `os.process.system()` with `start /b` to run in background.

### Clipboard Race Conditions
If another application holds the clipboard open, `os.clipboard.read()` / `write()` may return `""` / `false`. Retry after a short delay if needed.

### Memory Reporting
`os.totalMemory()` and `os.freeMemory()` report physical RAM only. Virtual memory, pagefile, and GPU memory are not included.

### Exit Codes
`os.process.exit(0)` = success, any non-zero = error. Standard convention is `exit(1)` for generic errors.

---

## Full Example: System Info Dashboard

```ez
use "ezos"

out "===== System Information ====="
out "Platform : " + os.platform()

uptime = os.uptime()
out "Uptime   : " + str(floor(uptime / 3600)) + "h " + str(floor((uptime % 3600) / 60)) + "m"

total = os.totalMemory()
free = os.freeMemory()
used = total - free
usedPct = floor(used * 100 / total)

out "Total RAM: " + str(floor(total / 1073741824)) + " GB"
out "Used RAM : " + str(floor(used / 1048576)) + " MB (" + str(usedPct) + "%)"
out "Free RAM : " + str(floor(free / 1048576)) + " MB"
out "CWD      : " + os.cwd()
out "Username : " + os.env.get("USERNAME")
out "Temp dir : " + os.env.get("TEMP")
out "==============================="
```

---

## Full Example: .env Based App Config

```ez
use "ezos"

# Expects a .env file with:
# DB_HOST=localhost
# DB_PORT=5432
# API_KEY=secret-key-123
# DEBUG=true

ok = os.env.load(".env")
when not ok {
    out "Warning: .env file not found, using defaults"
    os.env.set("DB_HOST", "localhost")
    os.env.set("DB_PORT", "5432")
}

DB_HOST = os.env.get("DB_HOST")
DB_PORT = os.env.get("DB_PORT")
API_KEY = os.env.get("API_KEY")
DEBUG = os.env.get("DEBUG") == "true"

out "Connecting to " + DB_HOST + ":" + DB_PORT
when DEBUG { out "[DEBUG MODE ON]" }
```

---

## Full Example: Build Script

```ez
use "ezos"

out "=== EZ Build Script ==="

# Check compiler
result = os.process.exec("g++ --version")
when indexOf(result, "g++") == -1 {
    out "ERROR: g++ not found. Install MinGW."
    os.process.exit(1)
}

# Create output directory
os.process.system("mkdir build 2>nul")

# Compile
out "Compiling..."
code = os.process.system("g++ -O2 -o build/app src/*.cpp -lm")

when code == 0 {
    out "Build successful! Binary: build/app.exe"
    
    # Open output folder
    os.shell.open("build")
} other {
    out "Build FAILED with code " + str(code)
    os.process.exit(code)
}
```

---

## Full Example: Clipboard Code Formatter

```ez
use "ezos"

out "Reading code from clipboard..."
code = os.clipboard.read()

when len(code) == 0 {
    out "Clipboard is empty."
    os.process.exit(1)
}

# Simple formatter: add indentation markers
lines = split(code, "\n")
formatted = ""
indent = 0

get line in lines {
    trimmed = replace(line, "\r", "")
    
    # Decrease indent on closing braces
    when indexOf(trimmed, "}") == 0 and indent > 0 {
        indent = indent - 1
    }
    
    spaces = ""
    repeat i = 0 to indent - 1 { spaces = spaces + "  " }
    formatted = formatted + spaces + trimmed + "\n"
    
    # Increase indent after opening braces
    when indexOf(trimmed, "{") != -1 {
        indent = indent + 1
    }
}

os.clipboard.write(formatted)
out "Formatted code written back to clipboard. Paste it!"
```

---

*Documentation generated from `E:\ezlib\ezos\main.ez` — EZ OS Utilities Library*
