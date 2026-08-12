# os v2.0

Production-grade OS utilities for the [EZ Programming Language](https://github.com/imabd645/EZ-language). Pure EZ over Win32 FFI — modular layout, structured errors, and expanded APIs.

## Installation

```
ez install os
```

## Quick start

```ez
use "os"

out os.platform()          # windows
out os.hostname()
out os.homedir()
out os.version()["name"]

os.env.set("APP_ENV", "production")
os.env.load(".env", { "override": false, "expand": true })

cfgPath = os.path.resolve("config\\app.json")
out os.path.extname(cfgPath)

result = os.process.run("echo hello", { "check": false })
out result["stdout"]

os.shell.open("https://github.com/imabd645/EZ-language")
```

## Modules

| Module | Purpose |
|--------|---------|
| `os.env` | get/set/unset/has/require/all/expand/load/merge |
| `os.path` | join, joinAll, normalize, resolve, basename, dirname, extname, stem |
| `os.process` | exec, run (structured result), system, pid, exit |
| `os.clipboard` | read, write, clear |
| `os.shell` | open, explore, reveal |
| `os.dirs` | home, temp, appData, localAppData, programFiles, desktop, ... |
| Top-level `os.*` | platform, memory, arch, pid, version, cwd, sleep, ... |

## Layout

```
os/
  main.ez           Public facade + `export os`
  package.ez
  src/
    ffi.ez          All DLL bindings (internal)
    errors.ez       OSError, EnvError, ProcessError
    util.ez         Parsing + path helpers
    env.ez
    path.ez
    process.ez
    clipboard.ez
    shell.ez
    system.ez
    dirs.ez
  test_os.ez
```

## `.env` format

Supports `KEY=VALUE`, `export KEY=VALUE`, `#` / `//` comments, quoted values, and inline comments.

## Requirements

- Windows x64
- EZ interpreter with FFI (`os_load_lib`, `os_call`, ...)
- `fs` package (for `.env` file existence checks)

## License

MIT
