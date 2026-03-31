# EZ PDF Library v1.0

Professional PDF generation library for the EZ programming language. Create PDFs with text, lines, tables, and headers using a simple, chainable API.

## 📦 Installation

```bash
ez install pdf
```

Or install locally:
```bash
ez install .
```

## ✨ Features

- ✅ Create multi-page PDFs
- ✅ Add text with custom sizes and positions
- ✅ Draw lines and shapes
- ✅ Built-in table support with headers
- ✅ Professional document headers
- ✅ Chainable method calls for clean code
- ✅ Standard PDF coordinates (bottom-left origin, A4 size)

## 🚀 Quick Start

### Basic Example

```ez
use "pdf"

// Create a new PDF
doc = PDF.init("hello.pdf")

// Add a page and some text
doc.addPage()
   .text(100, 700, "Hello, World!", 24)
   .text(100, 650, "This is my first PDF!", 12)
   .save()
```

### Professional Document

```ez
use "pdf"

// Create invoice
invoice = PDF.init("invoice.pdf")

invoice.addPage()
       .header("INVOICE #2024-001")
       .text(50, 750, "Company Name", 14)
       .text(50, 730, "123 Business St.", 10)
       .text(50, 710, "City, State 12345", 10)
       .save()
```

### Tables

```ez
use "pdf"

// Create report with table
report = PDF.init("report.pdf")

report.addPage()
      .header("Monthly Sales Report")
      
// Table header
columns = ["Product", "Quantity", "Price", "Total"]
xPos = [50, 200, 300, 400]
report.tableHeader(700, columns, xPos)

// Table rows
report.tableRow(680, ["Laptop", "5", "$999", "$4,995"], xPos)
      .tableRow(660, ["Mouse", "20", "$25", "$500"], xPos)
      .tableRow(640, ["Keyboard", "15", "$75", "$1,125"], xPos)
      .save()
```

## 📚 API Reference

### Constructor

#### `PDF.init(filename)`
Creates a new PDF document.

**Parameters:**
- `filename` (string) - Output PDF file path

**Returns:** PDF object for chaining

**Example:**
```ez
doc = PDF.init("output.pdf")
```

---

### Core Methods

#### `addPage()`
Adds a new page to the PDF (A4 size: 595×842 points).

**Returns:** Self (for chaining)

**Example:**
```ez
doc.addPage()
   .addPage()  // Multiple pages
```

---

#### `text(x, y, txt, size)`
Adds text at specified coordinates.

**Parameters:**
- `x` (number) - X coordinate from left (0-595)
- `y` (number) - Y coordinate from bottom (0-842)
- `txt` (string) - Text content
- `size` (number) - Font size in points

**Returns:** Self (for chaining)

**Example:**
```ez
doc.text(100, 700, "Hello World", 16)
```

**Coordinate System:**
```
(0, 842) ─────────────────── (595, 842)  ← Top
    │                              │
    │        Page Content          │
    │                              │
(0, 0) ───────────────────── (595, 0)    ← Bottom
```

---

#### `line(x1, y1, x2, y2)`
Draws a line between two points.

**Parameters:**
- `x1`, `y1` (number) - Start point coordinates
- `x2`, `y2` (number) - End point coordinates

**Returns:** Self (for chaining)

**Example:**
```ez
doc.line(50, 700, 550, 700)  // Horizontal line
   .line(50, 700, 50, 100)   // Vertical line
```

---

#### `save()`
Saves the PDF to disk.

**Returns:** Nothing

**Example:**
```ez
doc.save()  // Must be called last!
```

---

### Professional Helper Methods

#### `header(title)`
Adds a professional document header with title and underline.

**Parameters:**
- `title` (string) - Header text

**Returns:** Self (for chaining)

**Visual:**
```
     Large Title Text (24pt)
─────────────────────────────
```

**Example:**
```ez
doc.header("Annual Report 2024")
```

---

#### `tableHeader(y, columns, xPos)`
Creates a formatted table header row with underline.

**Parameters:**
- `y` (number) - Y coordinate for header
- `columns` (array) - Column names
- `xPos` (array) - X positions for each column

**Returns:** Self (for chaining)

**Example:**
```ez
columns = ["Name", "Age", "City"]
xPos = [50, 200, 350]
doc.tableHeader(700, columns, xPos)
```

---

#### `tableRow(y, data, xPos)`
Adds a data row to a table.

**Parameters:**
- `y` (number) - Y coordinate for row
- `data` (array) - Cell values
- `xPos` (array) - X positions (same as header)

**Returns:** Self (for chaining)

**Example:**
```ez
doc.tableRow(680, ["Alice", "25", "NYC"], xPos)
   .tableRow(660, ["Bob", "30", "LA"], xPos)
```

---

## 📖 Complete Examples

### 1. Simple Letter

```ez
use "pdf"

letter = PDF.init("letter.pdf")

letter.addPage()
      .text(50, 750, "Dear Sir/Madam,", 12)
      .text(50, 720, "I am writing to inform you...", 11)
      .text(50, 690, "Best regards,", 11)
      .text(50, 670, "John Doe", 11)
      .save()
```

### 2. Business Card

```ez
use "pdf"

card = PDF.init("business_card.pdf")

card.addPage()
    .text(100, 500, "JOHN DOE", 20)
    .text(100, 475, "Software Engineer", 12)
    .line(100, 470, 300, 470)
    .text(100, 450, "john@example.com", 10)
    .text(100, 435, "+1 (555) 123-4567", 10)
    .save()
```

### 3. Invoice

```ez
use "pdf"

invoice = PDF.init("invoice_001.pdf")

invoice.addPage()
       .header("INVOICE #001")
       
// Company info
invoice.text(50, 750, "ABC Company Inc.", 14)
       .text(50, 730, "123 Business Ave", 10)
       .text(50, 710, "New York, NY 10001", 10)

// Client info
invoice.text(400, 750, "Bill To:", 12)
       .text(400, 730, "Client Name", 11)
       .text(400, 710, "456 Client St", 10)

// Table
cols = ["Description", "Qty", "Price", "Total"]
xPos = [50, 300, 400, 480]

invoice.tableHeader(650, cols, xPos)
       .tableRow(630, ["Consulting", "10", "$150", "$1,500"], xPos)
       .tableRow(610, ["Development", "20", "$200", "$4,000"], xPos)
       .line(50, 590, 550, 590)
       .text(400, 570, "TOTAL: $5,500", 14)
       .save()
```

### 4. Multi-Page Report

```ez
use "pdf"

report = PDF.init("quarterly_report.pdf")

// Page 1 - Cover
report.addPage()
      .text(200, 500, "Q4 2024 Report", 32)
      .text(220, 450, "Sales Analysis", 18)

// Page 2 - Data
report.addPage()
      .header("Sales Data")
      .text(50, 720, "Total Revenue: $125,000", 12)
      .text(50, 700, "Growth Rate: 23%", 12)

// Page 3 - Table
report.addPage()
      .header("Regional Breakdown")

regions = ["Region", "Sales", "Growth"]
xPos = [50, 250, 400]

report.tableHeader(720, regions, xPos)
      .tableRow(700, ["North", "$45,000", "18%"], xPos)
      .tableRow(680, ["South", "$38,000", "25%"], xPos)
      .tableRow(660, ["East", "$25,000", "15%"], xPos)
      .tableRow(640, ["West", "$17,000", "35%"], xPos)
      .save()
```

### 5. Certificate

```ez
use "pdf"

cert = PDF.init("certificate.pdf")

cert.addPage()
    // Border
    .line(50, 50, 545, 50)
    .line(50, 792, 545, 792)
    .line(50, 50, 50, 792)
    .line(545, 50, 545, 792)
    
    // Content
    .text(200, 700, "CERTIFICATE", 28)
    .text(180, 650, "This certifies that", 14)
    .text(220, 600, "John Smith", 20)
    .text(150, 550, "has completed the course", 14)
    .text(180, 500, "EZ Programming 101", 18)
    .text(100, 200, "Instructor: _________________", 12)
    .text(350, 200, "Date: _________________", 12)
    .save()
```

## 📐 Coordinate Reference

### A4 Page Dimensions
- **Width:** 595 points (210mm)
- **Height:** 842 points (297mm)
- **Origin:** Bottom-left corner (0, 0)

### Common Y Positions
```
842 ─ Top margin
800 ─ Header area
750 ─ Title/main content start
700 ─ Body content
400 ─ Mid-page
200 ─ Footer area
50  ─ Bottom margin
0   ─ Page bottom
```

### Common X Positions
```
0    ─ Left edge
50   ─ Left margin
100  ─ First column (narrow margin)
297  ─ Center
500  ─ Right content area
550  ─ Right margin
595  ─ Right edge
```

## 🎨 Best Practices

### 1. Use Consistent Margins
```ez
// Define constants
LEFT_MARGIN = 50
RIGHT_MARGIN = 550
TOP_MARGIN = 800
BOTTOM_MARGIN = 50
```

### 2. Calculate Y Positions for Readability
```ez
y = 800
lineHeight = 20

doc.text(50, y, "Line 1", 12)
y = y - lineHeight
doc.text(50, y, "Line 2", 12)
y = y - lineHeight
doc.text(50, y, "Line 3", 12)
```

### 3. Use Helper Methods for Consistency
```ez
// Always use header() for titles
doc.header("My Document")

// Use tableHeader() and tableRow() for tables
// Don't manually position each cell
```

### 4. Chain Methods for Cleaner Code
```ez
// Good ✅
doc.addPage()
   .header("Title")
   .text(50, 700, "Content", 12)
   .save()

// Avoid ❌
doc.addPage()
doc.header("Title")
doc.text(50, 700, "Content", 12)
doc.save()
```

## 🐛 Common Issues

### Problem: Text Appears Outside Page
**Solution:** Check Y coordinate is between 0-842
```ez
// Wrong ❌
doc.text(50, 900, "Text", 12)  // Above page!

// Correct ✅
doc.text(50, 800, "Text", 12)
```

### Problem: Text Overlaps
**Solution:** Calculate proper spacing
```ez
y = 800
doc.text(50, y, "Line 1", 12)
y = y - 20  // Add spacing
doc.text(50, y, "Line 2", 12)
```

### Problem: Table Columns Misaligned
**Solution:** Use consistent xPos array
```ez
xPos = [50, 200, 350]  // Define once

doc.tableHeader(y, headers, xPos)
   .tableRow(y-20, data1, xPos)  // Use same xPos
   .tableRow(y-40, data2, xPos)
```

## 📄 License

MIT License - feel free to use in any project!

## 🤝 Contributing

Found a bug? Want to add features? Contributions welcome!

1. Fork the repository
2. Create a feature branch
3. Submit a pull request


## 📝 Changelog

### v1.0.0 (2026)
- ✅ Initial release
- ✅ Basic text and line drawing
- ✅ Professional helper methods
- ✅ Table support
- ✅ Multi-page documents
- ✅ Method chaining

---

Made by **Abduullah Masood**
