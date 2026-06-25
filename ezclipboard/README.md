# EZ Clipboard

System clipboard management via pure EZ FFI.

## Installation
```javascript
use "clipboard"
```

## Usage
```javascript
# Copy text to the clipboard
clipboard.copy("Hello World")

# Read text from the clipboard
text = clipboard.paste()
print(text)

# Clear the clipboard
clipboard.clear()
```
