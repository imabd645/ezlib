# ezos

> Comprehensive OS & System Utilities for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**ezos** is a pure-EZ standard library that exposes low-level operating system capabilities through FFI bindings to `Kernel32.dll`, `User32.dll`, `msvcrt.dll`, and `Shell32.dll` — no external dependencies required.

---

## Installation

```
ez install os
```

---

## Modules

| Module | Description |
|---|---|
| `os.env` | Read, write, and load environment variables |
| `os.path` | Path joining and inspection |
| `os.process` | Run shell commands and subprocesses |
| `os.clipboard` | Read and write the system clipboard |
| `os.shell` | Open files and URLs with the default application |

---

## Usage

### Environment Variables

```ez
use "os"

# Get a variable
home = os.env.get("USERPROFILE")
out home  # C:\Users\you

# Set a variable
os.env.set("MY_VAR", "hello")

# Load from a .env file (defaults to ".env" in cwd)
os.env.load(".env")
os.env.load("config/.env.production")
```

`.env` file format:
```
DB_HOST=localhost
DB_PORT=5432
API_KEY=my-secret-key
# This is a comment
```

---

### Path Utilities

```ez
use "os"

full = os.path.join("C:\\Users\\you", "Documents")
out full  # C:\Users\you\Documents

out os.path.isAbsolute("C:\\Windows")  # true
out os.path.isAbsolute("relative\\path")  # false
```

---

### Process & Shell Commands

```ez
use "os"

# Capture command output
result = os.process.exec("git log --oneline -5")
out result

# Run a command without capturing output (returns exit code)
code = os.process.system("mkdir build")

# Exit the program
os.process.exit(0)
```

---

### Clipboard

```ez
use "os"

# Write to clipboard
os.clipboard.write("Hello from EZ!")

# Read from clipboard
text = os.clipboard.read()
out text  # Hello from EZ!

# Clear the clipboard
os.clipboard.clear()
```

---

### Shell — Open Files & URLs

```ez
use "os"

# Open a file with its default application
os.shell.open("report.pdf")

# Open a URL in the default browser
os.shell.open("https://github.com/imabd645/EZ-language")

# Open a folder in Explorer
os.shell.open("C:\\Users\\you\\Documents")
```

---

### System Information

```ez
use "os"

out os.platform()       # windows
out os.uptime()         # seconds since boot (e.g. 3600.5)
out os.totalMemory()    # total RAM in bytes
out os.freeMemory()     # available RAM in bytes
out os.cwd()            # current working directory

# Change working directory
os.chdir("C:\\Projects\\myapp")
```

---

## API Reference

### `os.env`

| Method | Description |
|---|---|
| `os.env.get(key)` | Returns the value of an environment variable |
| `os.env.set(key, value)` | Sets an environment variable for the current process |
| `os.env.load(path?)` | Loads variables from a `.env` file (default: `".env"`) |

### `os.path`

| Method | Description |
|---|---|
| `os.path.join(p1, p2)` | Joins two path segments with `\` |
| `os.path.isAbsolute(path)` | Returns `true` if path is absolute (e.g. `C:\...`) |

### `os.process`

| Method | Description |
|---|---|
| `os.process.exec(cmd)` | Runs a command and returns its stdout as a string |
| `os.process.system(cmd)` | Runs a command and returns the exit code |
| `os.process.exit(code)` | Terminates the current EZ process |

### `os.clipboard`

| Method | Description |
|---|---|
| `os.clipboard.read()` | Returns the current clipboard text |
| `os.clipboard.write(text)` | Writes text to the clipboard |
| `os.clipboard.clear()` | Clears the clipboard |

### `os.shell`

| Method | Description |
|---|---|
| `os.shell.open(path)` | Opens a file, folder, or URL with the default application |

### `OS` (top-level)

| Method | Returns | Description |
|---|---|---|
| `os.platform()` | `"windows"` | Current OS platform |
| `os.uptime()` | `float` | System uptime in seconds |
| `os.totalMemory()` | `int` | Total installed RAM in bytes |
| `os.freeMemory()` | `int` | Available RAM in bytes |
| `os.cwd()` | `string` | Current working directory |
| `os.chdir(path)` | `bool` | Changes the working directory |

---

## Requirements

- **Platform**: Windows only (uses `Kernel32.dll`, `User32.dll`, `msvcrt.dll`, `Shell32.dll`)
- **EZ**: Any version supporting `os_load_lib`, `os_get_func`, and `os_call`

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
