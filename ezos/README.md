# EZ Mega OS Library

A massive systems utility module using strict OOP `models` for the EZ Programming Language.

## Getting Started

```ez
use "os"
```

## Models & Namespaces

### 1. General System Info
- `os.platform()` -> Returns "windows", "linux", or "darwin"
- `os.uptime()` -> Returns system uptime in seconds
- `os.totalMemory()` -> Returns total physical memory in bytes
- `os.freeMemory()` -> Returns available physical memory in bytes
- `os.cwd()` / `os.chdir(path)` -> Get or set current working directory

### 2. File & Path Routing (`os.path` / `os`)
- `os.readFile(path)`, `os.writeFile(path, str)`
- `os.mkdir(path)`, `os.exists(path)`
- `os.copyFile(src, dest)`, `os.listDir(path)`
- `os.deleteFile(path)`
- `os.path.join(p1, p2)` -> Constructs path properly
- `os.path.isAbsolute(p)`

### 3. Environment Variables (`os.env`)
- `os.env.get(key)`
- `os.env.set(key, val)`
- `os.env.load([file])` -> Powerful pure EZ `.env` parser. Automatically parses and sets environment variables into the OS. Call `os.env.load(".env")` to initialize!

### 4. Process Automation (`os.process`)
- `os.process.exec(cmd)` -> Runs a shell command completely hidden and **returns the terminal stdout** as a string. (e.g. `res = os.process.exec("ping google.com")`)
- `os.process.system(cmd)` -> Runs standard shell and outputs instantly, returns numeric exit code.
- `os.process.exit(code)` -> Instantly kills the EZ script execution.

### 5. Windows Shell & Clipboard (`os.clipboard` / `os.shell`)
- `os.clipboard.read()` -> Gets text from the global Windows clipboard
- `os.clipboard.write(text)` -> Sends text to the global Windows clipboard
- `os.clipboard.clear()`
- `os.shell.open(path)` -> Natively acts like double-clicking a file or URL in Windows Explorer.
