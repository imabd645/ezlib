# EZ PDF Generation Library (`ezpdf`)

`ezpdf` is a robust, full-featured PDF generation library for the EZ language. It uses the native `pdfgen` C library via FFI (Foreign Function Interface), providing blazingly fast PDF creation with full support for text, colors, drawing, and shapes.

## Installation

```bash
ez install ezpdf 1.0.0
```

## Basic Usage

To generate a PDF, you create a `PDF` object, add pages, add your content, and finally save it. 
Note: All coordinates (x, y) originate from the **bottom-left** of the page (standard PDF coordinate system).

```ez
use "ezpdf"

# 1. Create a PDF instance (A4 size by default)
doc = PDF("invoice.pdf")

# 2. Add a new page
doc.addPage()

# 3. Add text (x, y, text, size, color)
doc.text(50, 750, "Hello World!", 16, doc.rgb(0,0,0))

# 4. Save and export the PDF
doc.save()
```

## Styling and Colors

Colors are packed into 32-bit integers. You can generate them easily using the `rgb()` helper:

```ez
RED = doc.rgb(255, 0, 0)
BLUE = doc.rgb(0, 0, 255)
GREEN = doc.rgb(0, 255, 0)
```

You can change the font at any time before drawing text. Supported standard fonts include:
- `"Helvetica"`
- `"Helvetica-Bold"`
- `"Helvetica-Oblique"`
- `"Times-Roman"`
- `"Courier"`

```ez
doc.setFont("Helvetica-Bold")
doc.text(50, 700, "Bold Text", 12, RED)
```

## Drawing Shapes and Lines

`ezpdf` supports a wide array of geometric shapes:

```ez
# Draw a Line: (x1, y1, x2, y2, thickness, color)
doc.line(50, 600, 200, 600, 2, BLUE)

# Draw an Empty Rectangle: (x, y, w, h, borderThickness, color)
doc.rect(50, 500, 100, 50, 1, RED)

# Draw a Filled Rectangle: (x, y, w, h, fillColor, borderColor, borderThickness)
doc.filledRect(200, 500, 100, 50, GREEN, RED, 2)

# Draw a Circle: (x, y, radius, borderThickness, borderColor, fillColor)
# (Pass 0 for fillColor to make it transparent)
doc.circle(100, 400, 40, 2, BLUE, 0)
```

## Embedding Images

You can embed local image files (`.jpg`, `.png`, `.bmp`) directly into the PDF:

```ez
# Embed an image: (x, y, width, height, path)
doc.image(50, 200, 150, 100, "logo.png")
```

## Professional Helpers

`ezpdf` includes built-in helper functions to rapidly build reports and invoices:

```ez
# Generates a large bold title with an underline
doc.header("Monthly Sales Report")

# Generates a table header row with an underline
doc.tableHeader(700, ["Item", "Quantity", "Price"], [50, 200, 400])

# Generates a standard data row 
doc.tableRow(680, ["Apples", 50, "$2.00"], [50, 200, 400])
```

## Architecture

`ezpdf` is powered by the `pdfgen` C library, compiled natively as a 64-bit DLL (`dll\ezpdf.dll`). It utilizes EZ's `os_load_lib` and `os_call` for high-performance memory-safe FFI binding.
