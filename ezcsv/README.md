# CSV Library

A robust CSV (Comma-Separated Values) parser and stringifier for EZ, fully supporting RFC 4180 rules, including:
- Commas inside fields
- Newlines inside fields
- Escaped quotes (`""`)

## Installation

This library is included in the standard `ezlib` packages. Import it via:
```ez
use "csv"
```

## Functions

### `parse(csvString)`
Converts a CSV string into an Array of Dicts. The first row of the CSV is used as the keys for each dictionary.
```ez
dataStr = "name,age\nAlice,30\nBob,25"
rows = csv.parse(dataStr)
out rows[0]["name"] # "Alice"
```

### `stringify(dataArray)`
Converts an Array of Dicts back into a CSV string. Automatically escapes fields that contain quotes, commas, or newlines.
```ez
data = [
    { "name": "Alice", "quote": "She said, \"Hello!\"" },
    { "name": "Bob", "quote": "No quote" }
]
csvStr = csv.stringify(data)
# name,quote
# Alice,"She said, ""Hello!"""
# Bob,No quote
```

### `read(filePath)`
Reads a CSV file directly from disk and parses it into an Array of Dicts.
```ez
data = csv.read("data.csv")
```

### `write(filePath, dataArray)`
Writes an Array of Dicts directly to a CSV file.
```ez
success = csv.write("output.csv", data)
```
