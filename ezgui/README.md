# EZ GUI Library v4.0 - Complete Edition

A comprehensive, elegant, object-oriented GUI framework for the EZ programming language. Build beautiful desktop applications with 20+ widgets, advanced layouts, dialogs, timers, and themes.

![Version](https://img.shields.io/badge/version-4.0-blue)
![Widgets](https://img.shields.io/badge/widgets-20+-green)
![Layouts](https://img.shields.io/badge/layouts-VBox%20%7C%20Grid-orange)
![Themes](https://img.shields.io/badge/themes-5-purple)

## 🚀 What's New in v4.0

- ✨ **10+ New Widgets** - Checkbox, Radio, Slider, ProgressBar, ListView, TreeView, DatePicker, Spinner, Image
- 📐 **Grid Layout** - Automatic grid-based positioning
- 🎨 **5 Built-in Themes** - Default, Neon, Retro, Ocean, Custom
- 🔔 **Timer Support** - Periodic callbacks
- 💬 **Advanced Dialogs** - Confirm, Input, File picker, Folder picker, Color picker
- 🎯 **Enhanced Widget API** - Enable/disable, position, style helpers
- 🖱️ **Event System** - onClick, onChange callbacks for all widgets

---

## 📦 Installation

```bash
ez install gui
```

Or install locally:
```bash
ez install .
```

---

## ✨ Features Overview

### 20+ Professional Widgets
- **Basic:** Label, Button, Input, Checkbox, Radio
- **Selection:** Dropdown, ListView, TreeView
- **Input:** Slider, Spinner, DatePicker
- **Display:** ProgressBar, Image, Panel, ScrollPanel, Tabs
- **Containers:** Window, Panel, ScrollPanel

### Smart Layouts
- **VBox** - Automatic vertical stacking
- **Grid** - Automatic grid positioning (rows × columns)

### 13 Built-in Colors
`white`, `black`, `gray`, `dark`, `light`, `red`, `green`, `blue`, `neon`, `yellow`, `purple`, `orange`, custom RGB

### 5 Theme Presets
- **Default** - Clean, professional
- **Neon** - Cyberpunk cyan
- **Retro** - Warm orange tones
- **Ocean** - Cool blue palette
- **Custom** - Define your own

### Advanced Dialogs
- Alert, Confirm, Input prompt
- File open/save dialogs
- Folder picker
- Color picker

---

## 🚀 Quick Start

### Hello World
```ez
use "gui"

win = gui.window("Hello EZ", 400, 300)

win.label("Hello, World!", 50, 100, 300, 50)
   .color("blue")
   .font("Arial", 24)

win.run()
```

### Interactive Button
```ez
use "gui"

win = gui.window("Click Me", 300, 200)

counter = 0
label = win.label("Clicks: 0", 50, 50, 200, 30)

win.button("Click Here!", 50, 100, 200, 40, || {
    counter = counter + 1
    label.text("Clicks: " + str(counter))
}).color("green")

win.run()
```

### Complete Form
```ez
use "gui"

win = gui.window("User Form", 400, 500)

vb = win.vbox(10)
vb.addSpace(10)

vb.label("Registration Form", 380, 40).font("Arial", 20)

vb.label("Name:", 380, 25)
nameIn = vb.input(380, 35)

vb.label("Email:", 380, 25)
emailIn = vb.input(380, 35)

vb.label("Age:", 380, 25)
ageSpinner = vb.spinner(380, 35, 18, 100)

vb.label("Gender:", 380, 25)
maleRadio = vb.radio("Male", 180, 30, 1)
femaleRadio = vb.radio("Female", 180, 30, 0)

agreeCheck = vb.checkbox("I agree to terms", 380, 30)

vb.button("Submit", 380, 50, || {
    when agreeCheck.checked() {
        name = nameIn.value()
        email = emailIn.value()
        age = ageSpinner.value()
        gui.alert("Success", "Registered: " + name)
    } other {
        gui.alert("Error", "Please agree to terms")
    }
}).color("blue")

win.run()
```

---

## 📚 Complete Widget Reference

### Window

#### `gui.window(title, width, height)`
Creates main application window.

```ez
win = gui.window("My App", 800, 600)
```

**Methods:**
- `win.run()` - Start event loop (call last)
- `win.setTimer(ms, callback)` - Create periodic timer
- `win.killTimer(id)` - Stop timer
- All widget creation methods (see below)

---

### Basic Widgets

#### Label
Display text.

```ez
label = win.label("Hello", x, y, width, height)
label.text("New text")        // Update text
label.font("Arial", 16)       // Set font
label.color("blue")           // Set color
```

---

#### Button
Clickable button.

```ez
btn = win.button("Click", x, y, w, h, || {
    out "Clicked!"
})
btn.color("green")
btn.disable()  // Make unclickable
btn.enable()   // Re-enable
```

---

#### Input
Single-line text input.

```ez
input = win.input(x, y, width, height)
input.text("Default value")
value = input.value()  // Get current text
input.setText("New value")
```

---

#### Checkbox
Toggleable checkbox.

```ez
check = win.checkbox("Remember me", x, y, w, h)
check.setChecked(yes)     // Check it
isChecked = check.checked()  // Get state

check.onChange(|| {
    when check.checked() {
        out "Checked!"
    }
})
```

---

#### Radio Button
Radio button (one selection per group).

```ez
// First radio starts group
option1 = win.radio("Option 1", x, y, w, h, 1)  
option2 = win.radio("Option 2", x, y+30, w, h, 0)
option3 = win.radio("Option 3", x, y+60, w, h, 0)

when option1.selected() {
    out "Option 1 selected"
}
```

---

#### Dropdown
Dropdown/ComboBox selection.

```ez
dropdown = win.dropdown(x, y, width, height)
dropdown.add("Option 1")
        .add("Option 2")
        .add("Option 3")

selected = dropdown.selected()  // Get selected text
```

---

### Advanced Widgets

#### Slider
Horizontal slider for numeric input.

```ez
slider = win.slider(x, y, w, h, minValue, maxValue)
slider.setValue(50)
value = slider.value()

slider.onChange(|| {
    out "Value: " + str(slider.value())
})
```

---

#### ProgressBar
Visual progress indicator.

```ez
progress = win.progressBar(x, y, width, height)
progress.setRange(0, 100)
progress.setValue(75)  // 75%
```

---

#### Spinner
Numeric spinner (up/down arrows).

```ez
spinner = win.spinner(x, y, w, h, minValue, maxValue)
spinner.setValue(25)
age = spinner.value()
```

---

#### DatePicker
Calendar date picker.

```ez
datePicker = win.datePicker(x, y, width, height)
datePicker.setValue(2024, 3, 15)  // Year, Month, Day
date = datePicker.value()  // Returns "2024-03-15"
```

---

#### ListView
Multi-column list with rows.

```ez
list = win.listView(x, y, width, height)

// Add columns
list.addColumn("Name", 150)
    .addColumn("Age", 80)
    .addColumn("City", 120)

// Add rows
list.addRow(["Alice", "25", "NYC"])
list.addRow(["Bob", "30", "LA"])

// Get data
selectedIndex = list.selected()
rowData = list.getRow(0)  // Returns ["Alice", "25", "NYC"]

// Remove
list.removeRow(1)
list.clear()
```

---

#### TreeView
Hierarchical tree structure.

```ez
tree = win.treeView(x, y, width, height)

root = tree.add(0, "Root")           // 0 = top level
child1 = tree.add(root, "Child 1")
child2 = tree.add(root, "Child 2")
grandchild = tree.add(child1, "Grandchild")

tree.onChange(|| {
    selected = tree.selected()
    out "Selected: " + selected
})
```

---

#### Image
Display image from file.

```ez
img = win.image("photo.png", x, y, width, height)
img.load("different.jpg")  // Change image
```

---

### Container Widgets

#### Panel
Container for grouping widgets.

```ez
panel = win.panel(x, y, width, height)
panel.color("light")

// Add widgets to panel
panel.label("Inside Panel", 10, 10, 100, 30)
panel.button("Click", 10, 50, 80, 30, || {
    out "Button in panel clicked"
})
```

---

#### ScrollPanel
Scrollable container for large content.

```ez
scroll = win.scrollPanel(x, y, width, height)
scroll.contentSize(width, 2000)  // Content height

// Add many items
repeat i = 0 to 49 {
    scroll.label("Item " + str(i), 10, i*40, 200, 30)
}
```

---

#### Tabs
Tabbed interface.

```ez
tabs = win.tabs(x, y, width, height, || {
    currentTab = tabs.selected()
    out "Switched to tab: " + str(currentTab)
})

tabs.add("Home")
    .add("Settings")
    .add("About")

// Show content based on selected tab
when tabs.selected() == 0 {
    // Show home content
}
```

---

## 🎨 Styling & Theming

### Colors

#### Built-in Color Names
```ez
widget.color("blue")    // Background blue, text white
widget.color("green")   // Background green, text white
widget.color("red")     // Background red, text white
widget.color("light")   // Light gray background
widget.color("dark")    // Dark gray, text white
widget.color("gray")    // Medium gray
widget.color("neon")    // Cyan
widget.color("yellow")  // Yellow
widget.color("orange")  // Orange
widget.color("purple")  // Purple, text white
```

#### Style Helper
```ez
// Combined styling
widget.style(
    "light",        // Background color
    "dark",         // Foreground (text) color
    "Arial",        // Font name
    16              // Font size
)

// Partial styling (use 0 to skip)
widget.style("blue", 0, 0, 0)  // Only background
widget.style(0, "white", 0, 0) // Only text color
widget.style(0, 0, "Consolas", 12)  // Only font
```

### Themes

#### Apply Global Theme
```ez
use "gui"

gui.setTheme("neon")    // Cyberpunk cyan theme
gui.setTheme("retro")   // Warm orange theme
gui.setTheme("ocean")   // Cool blue theme

win = gui.window("Themed App", 600, 400)
```

**Theme Effects:**
- **Neon** - Dark blue-black background, cyan accents
- **Retro** - Warm brown/orange palette
- **Ocean** - Deep blue with light blue accents

---

## 📐 Layout Systems

### VBox Layout
Automatic vertical stacking.

```ez
vb = win.vbox(10)  // 10px spacing

vb.label("Name:", 300, 25)
vb.input(300, 35)
vb.label("Email:", 300, 25)
vb.input(300, 35)
vb.addSpace(20)  // Extra gap
vb.button("Submit", 300, 50, || {
    out "Submitted"
})
```

**VBox Methods:**
- `label(text, width, height)`
- `button(text, width, height, callback)`
- `input(width, height)`
- `dropdown(width, height)`
- `checkbox(text, width, height)`
- `radio(text, width, height, groupStart)`
- `slider(width, height, min, max)`
- `progressBar(width, height)`
- `listView(width, height)`
- `treeView(width, height)`
- `datePicker(width, height)`
- `spinner(width, height, min, max)`
- `image(path, width, height)`
- `addSpace(height)` - Add vertical spacing

---

### Grid Layout
Automatic grid positioning.

```ez
grid = win.grid(3, 10, 10)  // 3 columns, 10px padding
grid.setCellSize(100, 40)    // Each cell 100×40

// Automatically positions in grid
grid.button("1", || { out "1" })
grid.button("2", || { out "2" })
grid.button("3", || { out "3" })
grid.button("4", || { out "4" })  // Wraps to next row
grid.button("5", || { out "5" })

// Skip cells
grid.skipCell()
grid.button("7", || { out "7" })

// Force new row
grid.newRow()
```

**Grid Methods:**
- `setCellSize(width, height)` - Set uniform cell size
- `label(text)` - Add label
- `button(text, callback)` - Add button
- `input()` - Add input
- `checkbox(text)` - Add checkbox
- `dropdown()` - Add dropdown
- `spinner(min, max)` - Add spinner
- `image(path)` - Add image
- `skipCell()` - Leave cell empty
- `newRow()` - Force next row

---

## 💬 Dialogs & User Input

### Alert Dialog
```ez
gui.alert("Success", "Operation completed!")
gui.alert("Single argument shows as message")
```

### Confirm Dialog
```ez
result = gui.dialog.confirm("Confirm", "Are you sure?")
when result {
    out "User confirmed"
} other {
    out "User cancelled"
}
```

### Input Dialog
```ez
name = gui.dialog.input("Enter Name", "What's your name?")
when name {
    out "Hello, " + name
}
```

### File Dialogs
```ez
// Open file
filename = gui.dialog.openFile("Select File", "*.txt")
when filename {
    content = readFile(filename)
}

// Save file
filename = gui.dialog.saveFile("Save As", "*.txt")
when filename {
    writeFile(filename, data)
}

// Open folder
folder = gui.dialog.openFolder()
when folder {
    files = listDir(folder)
}
```

### Color Picker
```ez
color = gui.dialog.colorPicker()
// Returns "#RRGGBB" format
when color {
    out "Selected: " + color
}
```

---

## ⏱️ Timers

### Create Timer
```ez
win = gui.window("Timer Demo", 400, 300)

counter = 0
label = win.label("0", 150, 100, 100, 50)

timerId = win.setTimer(1000, || {  // Every 1000ms
    counter = counter + 1
    label.text(str(counter))
})

// Stop timer
win.button("Stop", 150, 200, 100, 40, || {
    win.killTimer(timerId)
})

win.run()
```

---

## 🎯 Complete Examples

### 1. Calculator

```ez
use "gui"

win = gui.window("Calculator", 300, 400)

display = win.input(10, 10, 280, 40)
display.font("Consolas", 18)

current = "0"
operator = ""
firstNum = "0"

task updateDisplay(val) {
    display.text(val)
}

task numberClick(n) {
    when current == "0" {
        current = n
    } other {
        current = current + n
    }
    updateDisplay(current)
}

task operatorClick(op) {
    firstNum = current
    operator = op
    current = "0"
}

task equals() {
    a = num(firstNum)
    b = num(current)
    result = 0
    
    when operator == "+" { result = a + b }
    when operator == "-" { result = a - b }
    when operator == "*" { result = a * b }
    when operator == "/" { result = a / b }
    
    current = str(result)
    updateDisplay(current)
}

// Number grid
grid = win.grid(3, 10, 10)
grid.setCellSize(60, 40)

repeat i = 1 to 9 {
    num = str(i)
    grid.button(num, || { numberClick(num) })
}

grid.button("0", || { numberClick("0") })
grid.button("C", || { current = "0" updateDisplay(current) })
grid.button("=", || { equals() }).color("green")

// Operators (manual positioning)
win.button("+", 220, 60, 60, 40, || { operatorClick("+") }).color("blue")
win.button("-", 220, 110, 60, 40, || { operatorClick("-") }).color("blue")
win.button("*", 220, 160, 60, 40, || { operatorClick("*") }).color("blue")
win.button("/", 220, 210, 60, 40, || { operatorClick("/") }).color("blue")

win.run()
```

---

### 2. Todo List with ListView

```ez
use "gui"

win = gui.window("Todo List", 500, 600)

// Input area
win.label("New Task:", 10, 10, 100, 25)
taskInput = win.input(110, 10, 280, 25)
win.button("Add", 400, 10, 80, 25, || {
    task = taskInput.value()
    when len(task) > 0 {
        list.addRow([task, "Pending"])
        taskInput.text("")
    }
}).color("green")

// List
list = win.listView(10, 50, 480, 500)
list.addColumn("Task", 350)
    .addColumn("Status", 120)

// Context menu buttons
win.button("Complete", 10, 560, 100, 30, || {
    idx = list.selected()
    when idx >= 0 {
        row = list.getRow(idx)
        list.removeRow(idx)
        list.addRow([row[0], "Done"])
    }
}).color("blue")

win.button("Delete", 120, 560, 100, 30, || {
    idx = list.selected()
    when idx >= 0 {
        list.removeRow(idx)
    }
}).color("red")

win.run()
```

---

### 3. Image Gallery with Grid

```ez
use "gui"

win = gui.window("Photo Gallery", 800, 600)

scroll = win.scrollPanel(10, 10, 780, 580)
scroll.contentSize(780, 2000)

grid = scroll.grid(4, 10, 10)  // 4 columns
grid.setCellSize(180, 180)

images = ["photo1.jpg", "photo2.jpg", "photo3.jpg", 
          "photo4.jpg", "photo5.jpg", "photo6.jpg"]

get img in images {
    grid.image(img)
}

win.run()
```

---

### 4. Settings Panel with Tabs

```ez
use "gui"

win = gui.window("Settings", 600, 500)

tabs = win.tabs(10, 10, 580, 480, || {
    // Tab changed
})

tabs.add("General")
    .add("Appearance")
    .add("Privacy")
    .add("About")

// Note: Create panels for each tab and show/hide based on tabs.selected()

win.run()
```

---

### 5. File Browser with TreeView

```ez
use "gui"

win = gui.window("File Browser", 600, 500)

tree = win.treeView(10, 50, 280, 440)

// Path bar
pathLabel = win.label("Path: /", 10, 10, 580, 30)

// Build tree
root = tree.add(0, "C:/")
docs = tree.add(root, "Documents")
pics = tree.add(root, "Pictures")
tree.add(docs, "Work")
tree.add(docs, "Personal")

// Content area
contentList = win.listView(300, 50, 290, 440)
contentList.addColumn("Name", 200)
           .addColumn("Size", 80)

tree.onChange(|| {
    selected = tree.selected()
    pathLabel.text("Path: " + selected)
    // Load content for selected folder
})

win.run()
```

---

### 6. Progress Tracker

```ez
use "gui"

win = gui.window("Download Manager", 500, 400)

vb = win.vbox(10)
vb.addSpace(10)

vb.label("File Downloads", 480, 40).font("Arial", 20)

vb.label("Download 1: document.pdf", 480, 25)
progress1 = vb.progressBar(480, 30)
progress1.setRange(0, 100)

vb.label("Download 2: video.mp4", 480, 25)
progress2 = vb.progressBar(480, 30)
progress2.setRange(0, 100)

vb.label("Download 3: archive.zip", 480, 25)
progress3 = vb.progressBar(480, 30)
progress3.setRange(0, 100)

vb.addSpace(20)
vb.button("Start Downloads", 480, 50, || {
    // Simulate download with timer
    prog = 0
    timerId = win.setTimer(100, || {
        prog = prog + 2
        progress1.setValue(prog)
        progress2.setValue(prog * 0.7)
        progress3.setValue(prog * 0.5)
        
        when prog >= 100 {
            win.killTimer(timerId)
            gui.alert("Complete", "All downloads finished!")
        }
    })
}).color("green")

win.run()
```

---

### 7. User Registration Form

```ez
use "gui"

win = gui.window("User Registration", 450, 700)

vb = win.vbox(10)
vb.addSpace(10)

vb.label("Create Account", 430, 50).font("Arial", 24).color("blue")

vb.label("Full Name:", 430, 25)
nameIn = vb.input(430, 35)

vb.label("Email:", 430, 25)
emailIn = vb.input(430, 35)

vb.label("Password:", 430, 25)
passIn = vb.input(430, 35)

vb.label("Confirm Password:", 430, 25)
confirmIn = vb.input(430, 35)

vb.label("Date of Birth:", 430, 25)
dobPicker = vb.datePicker(430, 35)

vb.label("Gender:", 430, 25)
maleRadio = vb.radio("Male", 200, 30, 1)
femaleRadio = vb.radio("Female", 200, 30, 0)

vb.label("Country:", 430, 25)
countryDrop = vb.dropdown(430, 35)
countryDrop.add("Pakistan").add("USA").add("UK").add("Canada")

agreeCheck = vb.checkbox("I agree to Terms & Conditions", 430, 30)

vb.addSpace(10)
vb.button("Register", 430, 50, || {
    // Validation
    when not agreeCheck.checked() {
        gui.alert("Error", "Please agree to terms")
        give 0
    }
    
    pass = passIn.value()
    confirm = confirmIn.value()
    
    when pass != confirm {
        gui.alert("Error", "Passwords don't match")
        give 0
    }
    
    when len(pass) < 8 {
        gui.alert("Error", "Password must be 8+ characters")
        give 0
    }
    
    name = nameIn.value()
    email = emailIn.value()
    dob = dobPicker.value()
    
    gui.alert("Success", "Welcome, " + name + "!")
}).color("green")

win.run()
```

---

## 🎨 Color Reference

| Color | RGB | Use Case | Auto Text Color |
|-------|-----|----------|-----------------|
| `white` | 255,255,255 | Backgrounds | Black |
| `black` | 0,0,0 | Text, borders | White |
| `gray` | 128,128,128 | Disabled | Black |
| `dark` | 40,40,40 | Dark theme | White |
| `light` | 240,240,240 | Panels | Black |
| `red` | 255,0,0 | Delete, errors | White |
| `green` | 0,200,0 | Success, submit | White |
| `blue` | 0,0,255 | Primary actions | White |
| `neon` | 0,255,255 | Highlights | Black |
| `yellow` | 255,255,0 | Warnings | Black |
| `purple` | 128,0,128 | Special | White |
| `orange` | 255,165,0 | Secondary | Black |

---

## 🐛 Common Issues & Solutions

### Widget Not Visible
```ez
// Make sure window.run() is last
win = gui.window("App", 400, 300)
// ... add widgets ...
win.run()  // ← Must be last line
```

### Can't Get Input Value
```ez
// Store reference to widget
input = win.input(10, 10, 200, 30)
// Later:
value = input.value()  // ✅ Works
```

### Dropdown Too Small
```ez
// Use sufficient height (150+ recommended)
drop = win.dropdown(10, 10, 200, 150)
```

### Grid Cells Overlapping
```ez
// Set cell size explicitly
grid = win.grid(3, 10, 10)
grid.setCellSize(100, 50)  // ← Add this
```

### Radio Buttons All Selected
```ez
// First radio should have groupStart=1
opt1 = win.radio("A", x, y, w, h, 1)  // ← Group start
opt2 = win.radio("B", x, y+30, w, h, 0)
opt3 = win.radio("C", x, y+60, w, h, 0)
```

---

## 💡 Best Practices

### 1. Use VBox for Forms
```ez
// ✅ Good - Automatic positioning
vb = win.vbox(10)
vb.label("Name:", 300, 25)
vb.input(300, 35)

// ❌ Avoid - Manual positioning errors
win.label("Name:", 10, 10, 100, 25)
win.input(10, 35, 200, 35)  // Easy to misalign
```

### 2. Store Widget References
```ez
// ✅ Good - Can update later
statusLabel = win.label("Ready", 10, 10, 200, 30)
// Later: statusLabel.text("Processing...")

// ❌ Avoid - Can't update
win.label("Ready", 10, 10, 200, 30)
```

### 3. Color Code Actions
```ez
win.button("Save", x, y, w, h, cb).color("green")
win.button("Delete", x, y, w, h, cb).color("red")
win.button("Cancel", x, y, w, h, cb).color("gray")
```

### 4. Use Layouts
```ez
// ✅ Good - Clean and maintainable
vb = win.vbox(10)
repeat i = 0 to 9 {
    vb.button("Item " + str(i), 300, 40, cb)
}

// ❌ Avoid - Manual positioning nightmare
repeat i = 0 to 9 {
    win.button("Item " + str(i), 10, 10 + i*50, 300, 40, cb)
}
```

### 5. Validate User Input
```ez
btn.click(|| {
    email = emailInput.value()
    
    when not contains(email, "@") {
        gui.alert("Error", "Invalid email")
        give 0
    }
    
    // Process valid email
})
```

---

## 📄 License

MIT License - Free for any use

## 🤝 Contributing

Contributions welcome! Report bugs or suggest features.

## 🔗 Related Libraries

- **db** - SQLite database
- **pdf** - PDF generation
- **datetime** - Date/time operations


---

Made with ❤️ for the EZ Language Community

**Full-featured GUI framework - From simple forms to complex applications** 🚀
