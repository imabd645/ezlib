# `fs` — File System Library for EZ

> Navigate, inspect, and manipulate the file system with a clean, minimal API.

---

## Overview

The `fs` library provides essential file system operations for EZ programs — directory creation, path inspection, file copying, moving, deletion, and metadata retrieval. All functions are thin wrappers over native system calls for reliable, fast I/O.

```ez
when not fs.exists("data/output") {
    fs.mkdir("data/output")
}

files = fs.listDir("data/output")
```

---

## API Reference

### `fs.mkdir(path)` → `bool`

Creates a directory at the given path. Creates intermediate directories as needed.

```ez
fs.mkdir("logs/2024/april")
```

---

### `fs.exists(path)` → `bool`

Returns `true` if the path exists (file or directory), `false` otherwise.

```ez
when fs.exists("config.ez") {
    say "Config found"
}
```

---

### `fs.isDir(path)` → `bool`

Returns `true` if the path exists and is a directory.

```ez
when fs.isDir("output") {
    say "output is a folder"
} other {
    say "output is a file or does not exist"
}
```

---

### `fs.listDir(path)` → `list`

Returns a list of entry names inside the given directory.

```ez
entries = fs.listDir("src")

loop entry in entries {
    say entry
}
```

> Returns filenames and subdirectory names — not full paths. Combine with the parent path manually if you need absolute or relative paths.

---

### `fs.copy(src, dest)` → `bool`

Copies a file from `src` to `dest`. Overwrites `dest` if it already exists.

```ez
fs.copy("template.html", "output/index.html")
```

---

### `fs.move(src, dest)` → `bool`

Moves (renames) a file or directory from `src` to `dest`.

```ez
fs.move("draft.txt", "published/post.txt")
```

---

### `fs.remove(path)` → `bool`

Deletes a file or directory at the given path.

> ⚠️ Deletion is permanent. There is no recycle bin or undo.

```ez
when fs.exists("temp.log") {
    fs.remove("temp.log")
}
```

---

### `fs.size(path)` → `number`

Returns the size of a file in **bytes**.

```ez
bytes = fs.size("video.mp4")
say "File size: " + bytes + " bytes"
```

---

### `fs.modifiedTime(path)` → `string`

Returns the last-modified timestamp of the file or directory as a string.

```ez
say fs.modifiedTime("notes.txt")
# => 2024-04-01 14:32:07
```

---

## Examples

### Ensure a Directory Exists Before Writing

```ez
outDir = "reports/2024"

when not fs.exists(outDir) {
    fs.mkdir(outDir)
}

# Safe to write files into outDir now
```

### List Only Files in a Directory

```ez
entries = fs.listDir("assets")

loop entry in entries {
    fullPath = "assets/" + entry
    when not fs.isDir(fullPath) {
        say "File: " + entry + " (" + fs.size(fullPath) + " bytes)"
    }
}
```

### Backup Before Overwriting

```ez
target = "config.json"

when fs.exists(target) {
    fs.copy(target, target + ".bak")
}

# Now safe to overwrite config.json
```

### Clean Up Temp Files

```ez
temps = ["tmp1.dat", "tmp2.dat", "cache.bin"]

loop f in temps {
    when fs.exists(f) {
        fs.remove(f)
        say "Removed: " + f
    }
}
```

### Inspect a Directory

```ez
path = "src"
entries = fs.listDir(path)

loop entry in entries {
    full = path + "/" + entry
    when fs.isDir(full) {
        say "[DIR]  " + entry
    } other {
        say "[FILE] " + entry + " — " + fs.size(full) + " bytes"
    }
}
```

---

## Quick Reference

| Function | Returns | Description |
|----------|---------|-------------|
| `fs.mkdir(path)` | `bool` | Create a directory |
| `fs.exists(path)` | `bool` | Check if path exists |
| `fs.isDir(path)` | `bool` | Check if path is a directory |
| `fs.listDir(path)` | `list` | List contents of a directory |
| `fs.copy(src, dest)` | `bool` | Copy a file |
| `fs.move(src, dest)` | `bool` | Move or rename a file |
| `fs.remove(path)` | `bool` | Delete a file or directory |
| `fs.size(path)` | `number` | Get file size in bytes |
| `fs.modifiedTime(path)` | `string` | Get last-modified timestamp |

---

## Notes

- All paths accept both forward slashes (`/`) and backslashes (`\`) on Windows.
- `fs.listDir` returns only the names of direct children — it does not recurse into subdirectories.
- `fs.move` can be used to rename files within the same directory.
- All functions return a falsy value or empty result on failure — always guard with `fs.exists` where correctness matters.

---

## License

Part of the EZ standard library — MIT License. See [LICENSE](./LICENSE).

---

*Simple paths. Solid foundations.*
