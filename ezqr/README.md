# ezqr

`ezqr` is a fast, native QR code generator library for EZ. It operates seamlessly by wrapping the C `libqrencode` library through EZ's native Foreign Function Interface (FFI). 

## Requirements
You must have the `qrencode` library installed on your system. 
On Windows with MSYS2, run:
```bash
pacman -S mingw-w64-x86_64-qrencode
```

## Quick Start

```ez
use "ezqr" as ezqr

# Create a QR code from text
# The second parameter is Error Correction Level:
# 0 = L, 1 = M, 2 = Q, 3 = H
qr = ezqr.QRCode("https://github.com/ez-lang/ez", 0)

# Render it directly to the terminal!
out qr.toAscii()
```

## API Reference

### `model QRCode`

#### `init(text, errorLevel)`
Constructs a new `QRCode` containing the specified data.

- `text` *(string)*: The data/text to encode into the QR code.
- `errorLevel` *(integer, optional)*: The error correction level.
  - `0`: Low (approx 7%)
  - `1`: Medium (approx 15%)
  - `2`: Quartile (approx 25%)
  - `3`: High (approx 30%)
  
If `errorLevel` is omitted, it defaults to `0`.

#### `toMatrix()`
Extracts the raw 2D array representation of the QR code.

- **Returns:** A 2D array (list of lists) of booleans. `true` means a black module, `false` means a white module. The array has dimensions `width` × `width`.

#### `toAscii()`
Generates an optimized string representation of the QR code for terminal output using UTF-8 half-block characters.

- **Returns:** A string containing the printable QR code, padded with a 2-module quiet zone border.

#### `saveImage(filepath, scale)`
Saves the generated QR code to disk as a high-quality, uncompressed 24-bit Bitmap (BMP) image. It uses FFI to write the binary file entirely natively without requiring any external image processing libraries.

- `filepath` *(string)*: The destination path (e.g., `"output.bmp"`).
- `scale` *(integer, optional)*: The scaling factor (pixels per QR module). Defaults to `10`. A standard QR code (version 1) has 21x21 modules, so a scale of 10 results in a 290x290 pixel image (including a 4-module quiet zone border).
- **Returns:** `true` if successful, throws an error if file access fails.
