# `os` — System Utilities Library for EZ

> One gateway to the operating system. Environment, processes, clipboard, paths, memory — all in one place.

---

## Overview

The `os` library is EZ's comprehensive interface to the underlying operating system. It provides a unified, object-oriented API for environment variables, process management, clipboard access, shell integration, system metrics, and file system shortcuts — all accessible from a single global `os` object.

```ez
say os.platform()        # => windows
say os.freeMemory()      # => 4294967296

os.env.load(".env")
secret = os.env.get("API_KEY")

os.clipboard.write("copied!")
```

---

## Structure

The `os` object is composed of five focused sub-modules, each handling a distinct area of system interaction:

| Property | Type | Responsibility |
|----------|------|----------------|
| `os.env` | `OSEnv` | Environment variables and `.env` file loading |
| `os.path` | `OSPath` | Path construction and inspection |
| `os.process` | `OSProcess` | Command execution and process control |
| `os.clipboard` | `OSClipboard` | System clipboard read/write |
| `os.shell` | `OSShell` | Shell-level file and URL opening |

---

## `os` — Top-Level Methods

### System Info

#### `os.platform()` → `string`
Returns the current operating system name.

```ez
say os.platform()   # => windows
```

#### `os.uptime()` → `number`
Returns the system uptime in seconds.

```ez
say os.uptime()   # => 43821
```

#### `os.totalMemory()` → `number`
Returns total installed RAM in bytes.

```ez
say os.totalMemory()   # => 17179869184  (16 GB)
```

#### `os.freeMemory()` → `number`
Returns currently available RAM in bytes.

```ez
say os.freeMemory()   # => 4294967296  (4 GB)
```

---

### Working Directory

#### `os.cwd()` → `string`
Returns the current working directory.

```ez
say os.cwd()   # => C:\Users\dev\myproject
```

#### `os.chdir(path)` → `bool`
Changes the current working directory.

```ez
os.chdir("C:\\projects\\ez-app")
```

---

### File System Shortcuts

These are convenience aliases over the `fs` module, accessible directly from `os`:

| Method | Equivalent |
|--------|-----------|
| `os.readFile(path)` | Read file contents as string |
| `os.writeFile(path, data)` | Write string to file |
| `os.mkdir(path)` | `fs.mkdir(path)` |
| `os.exists(path)` | `fs.exists(path)` |
| `os.copyFile(src, dest)` | `fs.copy(src, dest)` |
| `os.deleteFile(path)` | `fs.remove(path)` |
| `os.listDir(path)` | `fs.listDir(path)` |

```ez
when not os.exists("output") {
    os.mkdir("output")
}

data = os.readFile("input.txt")
os.writeFile("output/result.txt", data)
```

---

## `os.env` — Environment Variables

### `os.env.get(key)` → `string`
Returns the value of an environment variable.

```ez
home = os.env.get("USERPROFILE")
say home   # => C:\Users\dev
```

### `os.env.set(key, val)`
Sets an environment variable for the current process.

```ez
os.env.set("APP_MODE", "production")
```

### `os.env.load(path?)` → `bool`
Parses a `.env` file and loads its key-value pairs into the environment. Defaults to `.env` in the current directory if no path is given.

Returns `true` on success, `false` if the file does not exist.

```ez
os.env.load()              # loads .env
os.env.load("config/.env") # loads from a specific path

key = os.env.get("DATABASE_URL")
```

**`.env` file format:**
```env
# This is a comment
DATABASE_URL=postgres://localhost:5432/mydb
API_KEY=abc123
PORT=8080
```

**Parser behavior:**
- Lines beginning with `#` are treated as comments and skipped
- Keys and values are split on the first `=`
- Values containing `=` (e.g. Base64 strings, URLs) are handled correctly
- Windows line endings (`\r\n`) are stripped automatically
- Blank lines are ignored

---

## `os.path` — Path Utilities

### `os.path.join(p1, p2)` → `string`
Joins two path segments with the OS path separator.

```ez
full = os.path.join("C:\\projects", "myapp")
say full   # => C:\projects\myapp
```

### `os.path.isAbsolute(path)` → `bool`
Returns `true` if the path is absolute (e.g. starts with a drive letter like `C:`).

```ez
say os.path.isAbsolute("C:\\Users\\dev")   # => true
say os.path.isAbsolute("relative/path")    # => false
```

---

## `os.process` — Process & Command Execution

### `os.process.exec(cmd)` → `string`
Executes a shell command and **returns its output** as a string.

```ez
output = os.process.exec("dir")
say output
```

### `os.process.system(cmd)` → `number`
Runs a shell command and returns the **exit code**. Output goes directly to the terminal.

```ez
code = os.process.system("npm install")
when code != 0 {
    say "Command failed with code: " + code
}
```

### `os.process.exit(code)`
Terminates the EZ program with the given exit code.

```ez
os.process.exit(0)   # clean exit
os.process.exit(1)   # exit with error
```

---

## `os.clipboard` — System Clipboard

### `os.clipboard.read()` → `string`
Returns the current contents of the system clipboard.

```ez
text = os.clipboard.read()
say "Clipboard: " + text
```

### `os.clipboard.write(text)`
Writes text to the system clipboard.

```ez
os.clipboard.write("Hello from EZ!")
```

### `os.clipboard.clear()`
Clears the system clipboard.

```ez
os.clipboard.clear()
```

---

## `os.shell` — Shell Integration

### `os.shell.open(path)` → `bool`
Opens a file, folder, or URL using the system's default handler — the equivalent of double-clicking in Explorer.

```ez
os.shell.open("https://example.com")   # opens in default browser
os.shell.open("report.pdf")            # opens in default PDF viewer
os.shell.open("C:\\projects\\myapp")   # opens folder in Explorer
```

---

## Examples

### Load Config from `.env` and Use It

```ez
os.env.load()

mode = os.env.get("APP_MODE")
port = os.env.get("PORT")

say "Running in " + mode + " mode on port " + port
```

### Log System Info to a File

```ez
info = "Platform: " + os.platform() + "\n"
     + "Uptime:   " + os.uptime() + "s\n"
     + "Memory:   " + os.freeMemory() + " / " + os.totalMemory() + " bytes\n"
     + "CWD:      " + os.cwd()

os.writeFile("system_info.txt", info)
say "System info written."
```

### Run a Build Script and Handle Failure

```ez
say "Building..."
code = os.process.system("make build")

when code != 0 {
    say "Build failed. Exiting."
    os.process.exit(1)
}

say "Build succeeded."
os.shell.open("dist/")
```

### Copy Clipboard Contents to a File

```ez
contents = os.clipboard.read()

when contents != "" {
    os.writeFile("clipboard_dump.txt", contents)
    say "Saved clipboard to file."
} other {
    say "Clipboard is empty."
}
```

### Scaffold a Project Directory

```ez
base = "my-project"

dirs = [base, base + "\\src", base + "\\tests", base + "\\dist"]

loop dir in dirs {
    when not os.exists(dir) {
        os.mkdir(dir)
        say "Created: " + dir
    }
}

os.writeFile(base + "\\.env", "APP_MODE=development\nPORT=3000\n")
say "Project scaffolded at " + os.path.join(os.cwd(), base)
```

---

## Notes

- `os.env.load()` sets variables at the **process level** — they are accessible via `os.env.get()` for the duration of the EZ program.
- `os.process.exec()` captures stdout and returns it as a string. Use `os.process.system()` when you want live terminal output instead.
- `os.path.join()` currently uses `\` as the separator. For cross-platform path building, always use `os.path.join()` rather than string concatenation.
- `os.shell.open()` behavior depends on the system's file association settings.

---

## License

Part of the EZ standard library — MIT License. See [LICENSE](./LICENSE).

---

*One object. The whole OS.*
