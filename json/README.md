# JSON Library

A complete JSON (JavaScript Object Notation) parser and stringifier for EZ. Written entirely in pure EZ, it translates JSON text into EZ Arrays and Dictionaries, and vice versa.

## Installation

This library is included in the standard `ezlib` packages. Import it via:
```ez
use "json"
```

## Functions

### `parse(jsonString)`
Parses a JSON string and returns the equivalent EZ data structure (Dictionary, Array, string, number, boolean, or nil).
```ez
dataStr = "{\"name\": \"Alice\", \"skills\": [\"EZ\", \"C++\"]}"
obj = json.parse(dataStr)
out obj["skills"][0] # "EZ"
```

### `stringify(data)`
Converts an EZ data structure into a JSON string.
```ez
myDict = {
    "title": "EZ Language",
    "awesome": true,
    "version": 6.0
}
jsonStr = json.stringify(myDict)
```

### `read(filePath)`
Reads a JSON file directly from disk and parses it.
```ez
config = json.read("config.json")
```

### `write(filePath, data)`
Stringifies and writes EZ data directly to a JSON file.
```ez
success = json.write("output.json", config)
```
