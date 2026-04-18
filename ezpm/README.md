# ezpm

> Enhanced package manager for the [EZ Programming Language](https://github.com/imabd645/EZ-language)

**ezpm** is the official EZ package manager. It supports remote package registry resolution, semantic version constraints, lockfile pinning (`ez.lock`), SHA256 checksum verification, cached downloads, and recursive dependency installation — all written in pure EZ.

---

## Usage

```
ezpm install <package>[@version]   Install a package
ezpm install                       Install all packages from ez.lock
ezpm update <package>              Update a package to latest
ezpm remove <package>              Uninstall a package
ezpm list                          List installed packages
ezpm info <package>                Show package info and available versions
ezpm help                          Show help
```

---

## Installing Packages

### Install latest version

```
ezpm install math
ezpm install http
ezpm install regex
```

### Install a specific version

```
ezpm install http@2.0.0
```

### Install with version constraints

```
ezpm install crypto@^1.0.0     # compatible with major version 1.x.x
ezpm install collections@~1.2.0   # compatible with minor version 1.2.x
```

### Install from lockfile

When `ez.lock` exists, running `ezpm install` with no arguments restores all packages at their pinned versions — perfect for reproducible project setups:

```
ezpm install
```

---

## Version Constraints

| Constraint | Meaning |
|---|---|
| `@latest` | Most recent published version (default) |
| `@1.2.3` | Exact version — only this release |
| `@^1.0.0` | Any version with the same major (`1.x.x`) |
| `@~1.2.0` | Any version with the same major and minor (`1.2.x`) |

---

## Other Commands

### Update a package

```
ezpm update math
```

Fetches the latest version from the registry and installs it. The lockfile is updated automatically.

### Remove a package

```
ezpm remove math
```

Deletes the package from `C:/ezlib` and removes it from `ez.lock`.

### List installed packages

```
ezpm list
```

```
Installed packages:

  math @ 1.2.0
  http @ 2.0.0
  regex @ 1.0.0

Total: 3 packages
```

### Show package info

```
ezpm info http
```

```
Package: http
Description: Production-ready HTTP client for EZ
Latest: 2.0.0
Repository: https://github.com/imabd645/ezlib

Available versions:
  - 2.0.0
  - 1.1.0
  - 1.0.0

Installed locally: 2.0.0
```

---

## The Lockfile — `ez.lock`

Every install or update writes an `ez.lock` file to your project directory. It pins the exact resolved version, download URL, and SHA256 checksum for every installed package.

```json
{
  "version": "1.0.0",
  "timestamp": "2026-04-18T10:00:00",
  "packages": {
    "math": {
      "version": "1.2.0",
      "url": "https://raw.githubusercontent.com/imabd645/ezlib/main/ezmath/1.2.0/ezmath.zip",
      "sha256": "a3f1c2..."
    },
    "http": {
      "version": "2.0.0",
      "url": "https://raw.githubusercontent.com/imabd645/ezlib/main/ezhttp/2.0.0/ezhttp.zip",
      "sha256": "d9e4b1..."
    }
  }
}
```

Commit `ez.lock` to version control so your team always installs the same package versions.

---

## How It Works

### Installation flow

1. **Fetch index** — downloads `index.json` from the remote registry at `https://raw.githubusercontent.com/imabd645/ezlib/main`
2. **Resolve version** — applies your constraint against the available versions
3. **Check lockfile** — if `ez.lock` already pins this package, uses that version without hitting the registry
4. **Check cache** — if the package zip is already in `C:/ezlib/.cache`, skips the download
5. **Download** — fetches the package zip via `curl`
6. **Verify** — checks the SHA256 checksum if one is provided in the registry
7. **Extract** — unpacks to `C:/ezlib/<package>/<version>/`
8. **Link** — creates a `latest` junction at `C:/ezlib/<package>/latest`
9. **Recurse** — reads `package.ez` for dependencies and installs them the same way
10. **Write lockfile** — updates `ez.lock` with all resolved packages

### Directory structure

```
C:/ezlib/
├── .cache/               # Downloaded zip files
│   ├── math-1.2.0.zip
│   └── http-2.0.0.zip
├── math/
│   ├── 1.2.0/            # Versioned install
│   │   └── package.ez
│   └── latest/           # Junction → 1.2.0
└── http/
    ├── 2.0.0/
    │   └── package.ez
    └── latest/           # Junction → 2.0.0
```

---

## Using ezpm Programmatically

ezpm exposes its commands as an `ezpm` object for use inside EZ scripts:

```ez
use "ezpm"

# Install a package
ezpm.install("math@1.2.0")

# Update a package
ezpm.update("http")

# Remove a package
ezpm.remove("regex")

# List installed packages
ezpm.list()

# Get info about a package
ezpm.info("collections")
```

Convenience aliases are also available at the top level:

```ez
pmInstall("math")
pmUpdate("http")
pmRemove("regex")
pmList()
```

---

## `package.ez` Format

Each published package includes a `package.ez` manifest. ezpm reads this to resolve recursive dependencies:

```json
{
  "name": "ezorm",
  "version": "1.0.0",
  "description": "Object-Relational Mapper for EZ",
  "dependencies": {
    "db": "^1.0.0",
    "collections": "~1.2.0"
  }
}
```

---

## Requirements

- **Platform**: Windows (uses `C:/ezlib` as the global package directory, `mklink` for junctions, `curl` and `tar` for download/extraction)
- **EZ**: Any version with `http` and `fs` libraries available

---

## License

MIT — see the [EZ Language repository](https://github.com/imabd645/EZ-language) for details.
