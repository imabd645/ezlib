# ezpdf — PDF Generation Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezpdf"`  
> **File:** `E:\ezlib\ezpdf\main.ez`

---

## Overview

`ezpdf` provides a clean, chainable interface for generating PDF documents from EZ. It wraps the native `pdf_*` built-in functions with a professional helper API for headers, tables, and text layout.

---

## Quick Start

```ez
use "ezpdf"

doc = PDF("output.pdf")
doc.addPage()
doc.header("My Report")
doc.text(50, 760, "Hello, World!", 14)
doc.save()
```

---

## Coordinate System

EZ PDF uses standard PDF coordinates:
- **Origin** `(0, 0)` is the **bottom-left** corner of the page.
- **A4 page size:** `595 × 842` points (`[0 0 595 842]` MediaBox).
- **X** increases to the right, **Y** increases upward.

| Area | Y Range | Description |
|---|---|---|
| Top | ~800–842 | Header zone |
| Body | ~100–790 | Main content |
| Bottom | 0–100 | Footer zone |

---

## Model: `PDF`

### `PDF(filename)`

Creates and begins a new PDF document.

**Parameters:**
- `filename` — Output file path (e.g. `"report.pdf"`, `"C:\\docs\\invoice.pdf"`).

```ez
use "ezpdf"

doc = PDF("output.pdf")
# Calls pdf_begin("output.pdf") internally
```

---

### `doc.addPage()` → `self`

Adds a new page to the document. Call this before drawing content on each page.

```ez
use "ezpdf"

doc = PDF("multi.pdf")
doc.addPage()              # Page 1
doc.text(50, 800, "Page One", 14)

doc.addPage()              # Page 2
doc.text(50, 800, "Page Two", 14)

doc.save()
```

---

### `doc.text(x, y, txt, size)` → `self`

Draws text on the current page.

**Parameters:**
- `x` — X position in points from left edge.
- `y` — Y position in points from bottom edge.
- `txt` — Text string to render.
- `size` — Font size in points.

```ez
use "ezpdf"

doc = PDF("text.pdf")
doc.addPage()

# Title
doc.text(50, 800, "Annual Report 2025", 24)

# Body text
doc.text(50, 760, "This document summarizes our performance.", 12)
doc.text(50, 740, "Revenue grew by 35% year over year.", 12)

# Small note
doc.text(50, 50, "Confidential - Internal Use Only", 8)

doc.save()
```

---

### `doc.line(x1, y1, x2, y2)` → `self`

Draws a horizontal or diagonal line.

**Parameters:**
- `x1, y1` — Start point (PDF coordinates).
- `x2, y2` — End point (PDF coordinates).

```ez
use "ezpdf"

doc = PDF("lines.pdf")
doc.addPage()

# Full-width horizontal divider
doc.line(50, 790, 550, 790)

# Diagonal line
doc.line(50, 400, 550, 200)

# Vertical line (left margin)
doc.line(50, 100, 50, 800)

doc.save()
```

---

### `doc.save()`

Finalizes and writes the PDF file to disk. Prints confirmation to stdout.

```ez
use "ezpdf"

doc = PDF("final.pdf")
doc.addPage()
doc.text(50, 400, "Done!", 20)
doc.save()
# Outputs: "PDF Saved to: final.pdf"
```

> ⚠️ Always call `save()` — without it, the file is not written to disk.

---

## Helper Methods

### `doc.header(title)` → `self`

Renders a styled page header: bold title text at `(50, 800)` in size 24, followed by a horizontal divider line at `y=790`.

```ez
use "ezpdf"

doc = PDF("report.pdf")
doc.addPage()
doc.header("Q2 2025 Sales Report")
doc.text(50, 760, "Prepared by: Finance Department", 11)
doc.save()
```

---

### `doc.tableHeader(y, columns, xPos)` → `self`

Renders a table header row: draws each column label at the specified x positions, then draws a horizontal line below.

**Parameters:**
- `y` — Y position for the header row.
- `columns` — Array of column label strings.
- `xPos` — Array of X positions, one per column.

```ez
use "ezpdf"

doc = PDF("table.pdf")
doc.addPage()
doc.header("Product Inventory")

# Table header
cols = ["SKU", "Product Name", "Qty", "Unit Price", "Total"]
xPos = [50, 120, 320, 390, 470]
doc.tableHeader(750, cols, xPos)
doc.save()
```

---

### `doc.tableRow(y, data, xPos)` → `self`

Renders a single data row in a table.

**Parameters:**
- `y` — Y position for this row.
- `data` — Array of values (automatically converted via `str()`).
- `xPos` — Array of X positions, matching the column header positions.

```ez
use "ezpdf"

doc = PDF("inventory.pdf")
doc.addPage()
doc.header("Inventory Report")

cols = ["SKU", "Name", "Qty", "Price"]
xPos = [50, 140, 360, 430]
doc.tableHeader(750, cols, xPos)

rows = [
    ["WID-001", "Widget Pro", 125, "$9.99"],
    ["GAD-002", "Super Gadget", 47, "$24.99"],
    ["DON-003", "Premium Donut", 800, "$1.99"]
]

y = 730
get row in rows {
    doc.tableRow(y, row, xPos)
    y = y - 20
}

doc.save()
```

---

## Full Example: Invoice Generator

```ez
use "ezpdf"

task generateInvoice(invoiceNum, clientName, items) {
    doc = PDF("invoice_" + str(invoiceNum) + ".pdf")
    doc.addPage()
    
    # Header
    doc.text(50, 800, "INVOICE", 28)
    doc.line(50, 790, 550, 790)
    
    # Company info (top-right)
    doc.text(350, 800, "MyCompany Ltd.", 14)
    doc.text(350, 782, "123 Business St, City", 10)
    doc.text(350, 766, "info@mycompany.com", 10)
    
    # Invoice metadata
    doc.text(50, 760, "Invoice #: " + str(invoiceNum), 11)
    doc.text(50, 742, "Bill To: " + clientName, 11)
    doc.text(50, 724, "Date: 2025-06-11", 11)
    doc.text(50, 706, "Due: 2025-07-11", 11)
    
    doc.line(50, 698, 550, 698)
    
    # Table header
    cols = ["Description", "Qty", "Unit Price", "Amount"]
    xPos = [50, 300, 370, 460]
    doc.tableHeader(680, cols, xPos)
    
    # Table rows
    y = 660
    total = 0
    get item in items {
        amount = item["qty"] * item["price"]
        total = total + amount
        
        rowData = [item["desc"], item["qty"], "$" + str(item["price"]), "$" + str(amount)]
        doc.tableRow(y, rowData, xPos)
        y = y - 22
    }
    
    # Total line
    doc.line(50, y - 5, 550, y - 5)
    doc.text(370, y - 22, "TOTAL:", 12)
    doc.text(460, y - 22, "$" + str(total), 12)
    
    # Footer
    doc.line(50, 100, 550, 100)
    doc.text(50, 80, "Thank you for your business!", 10)
    doc.text(50, 64, "Payment due within 30 days. Late fee: 2% per month.", 9)
    
    doc.save()
    out "Invoice saved: invoice_" + str(invoiceNum) + ".pdf"
}

# Generate sample invoice
generateInvoice(1042, "Acme Corp", [
    {"desc": "EZ License (Enterprise)", "qty": 5, "price": 299},
    {"desc": "Support Package (1yr)", "qty": 1, "price": 499},
    {"desc": "Training Session", "qty": 3, "price": 150}
])
```

---

## Full Example: Report with Multiple Pages

```ez
use "ezpdf"

task generateReport(title, sections) {
    doc = PDF("report.pdf")
    
    # Page 1: Title Page
    doc.addPage()
    doc.text(200, 450, title, 28)
    doc.line(50, 430, 550, 430)
    doc.text(200, 400, "Generated: 2025-06-11", 12)
    doc.text(200, 380, "EZ Reporting System v1.0", 10)
    
    # Content Pages
    get section in sections {
        doc.addPage()
        doc.header(section["title"])
        
        y = 750
        get line in section["lines"] {
            doc.text(50, y, line, 11)
            y = y - 20
            when y < 100 { escape }   # Safety: don't overflow
        }
    }
    
    doc.save()
}

generateReport("Annual Summary 2025", [
    {
        "title": "Executive Summary",
        "lines": [
            "Revenue: $4.2M (+35% YoY)",
            "Net Income: $820K (+22% YoY)",
            "Total Customers: 1,847 (+18% YoY)",
            "Employee Headcount: 42 (+8 from last year)"
        ]
    },
    {
        "title": "Sales Performance",
        "lines": [
            "Q1 Sales: $890K",
            "Q2 Sales: $1.1M",
            "Q3 Sales: $1.05M",
            "Q4 Sales: $1.16M",
            "Top Region: North America (62% of revenue)",
            "Top Product: EZ Enterprise License"
        ]
    }
])
```

---

## Edge Cases & Important Notes

### Y Coordinate Direction
Y=0 is the **bottom** of the page, Y=842 is the top (A4). When adding content, start from the top (~800) and decrease Y for each new line. This is opposite of screen coordinates.

### No Automatic Word Wrap
Text does not wrap. Long strings will overflow the page horizontally. Manually split text or calculate line positions.

### No Font Support
The underlying `pdf_text` call uses the EZ runtime's built-in PDF renderer, which currently only supports one default font (Helvetica). Custom fonts are not supported in v1.0.

### Page Management
You must call `addPage()` before any drawing calls. If you call `text()` without a page, behavior is undefined (may crash or produce invalid PDF).

### Pixel vs Points
PDF points ≠ screen pixels. 1 point = 1/72 inch. A4 at 72 DPI = 595×842 points. Use these values for layout math.

### save() is Required
`PDF` uses `pdf_begin` to start the stream, and `pdf_save` to finalize it. Always call `save()` at the end or the file will be empty/invalid.

---

## Coordinate Quick Reference

```
Y = 842 ┌────────────────────────────────┐
        │   Header Zone (800-842)        │
Y = 790 ├────────────────────────────────┤ ← header() line
        │                                │
        │   Content Zone (120-790)       │
        │                                │
Y = 120 ├────────────────────────────────┤
        │   Footer Zone (50-120)         │
Y = 50  └────────────────────────────────┘
        X=50                          X=550
```

---

*Documentation generated from `E:\ezlib\ezpdf\main.ez` — EZ PDF Library v1.0*
