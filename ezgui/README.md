# EZ GUI Library

A high-level, object-oriented GUI framework for the EZ programming language. Build desktop applications with 20+ widgets, smart layouts, dialogs, timers, and themes — all in clean, readable EZ syntax.

---

## Installation

```bash
ez install gui
```

---

## Quick Start

```ez
use "gui"

win = gui.window("Hello EZ", 400, 300)

win.label("Hello, World!", 50, 100, 300, 50)
   .color("blue")
   .font("Arial", 24)

win.run()
```

> **Note:** Always call `win.run()` last — it starts the event loop.

---

## Table of Contents

- [Window](#window)
- [Widgets](#widgets)
  - [Label](#label)
  - [Button](#button)
  - [Input](#input)
  - [TextArea](#textarea)
  - [Checkbox](#checkbox)
  - [Radio](#radio)
  - [Dropdown](#dropdown)
  - [Slider](#slider)
  - [ProgressBar](#progressbar)
  - [Spinner](#spinner)
  - [DatePicker](#datepicker)
  - [ListView](#listview)
  - [TreeView](#treeview)
  - [Image](#image)
  - [Panel & ScrollPanel](#panel--scrollpanel)
  - [Tabs](#tabs)
  - [Toolbar & StatusBar](#toolbar--statusbar)
- [Layouts](#layouts)
  - [VBox](#vbox)
  - [HBox](#hbox)
  - [Grid](#grid)
- [Styling](#styling)
  - [Colors](#colors)
  - [Fonts](#fonts)
  - [Themes](#themes)
- [Dialogs](#dialogs)
- [Timers](#timers)
- [Events](#events)
- [Menus](#menus)
- [Examples](#examples)

---

## Window

Creates a top-level application window.

```ez
win = gui.window(title, width, height)
```

### Window Methods

| Method | Description |
|--------|-------------|
| `win.run()` | Start the event loop (call last) |
| `win.setTitle(text)` | Change the window title |
| `win.resize(w, h)` | Resize the window |
| `win.center()` | Center on screen |
| `win.getSize()` | Returns `[width, height]` |
| `win.getPos()` | Returns `[x, y]` |
| `win.setMinSize(w, h)` | Set minimum window size |
| `win.setAlwaysOnTop(v)` | Keep window above others |
| `win.setOpacity(a)` | Set opacity (0.0–1.0) |
| `win.setIcon(path)` | Set window icon |
| `win.minimize()` | Minimize |
| `win.maximize()` | Maximize |
| `win.restore()` | Restore from min/max |
| `win.onClose(cb)` | Callback when closed |
| `win.onResize(cb)` | Callback when resized |
| `win.onKeyPress(cb)` | Callback on key down |
| `win.onKeyUp(cb)` | Callback on key up |
| `win.onDrop(cb)` | Callback on file drop |

---

## Widgets

All widgets inherit these universal methods:

| Method | Description |
|--------|-------------|
| `widget.text(v)` | Set displayed text/value |
| `widget.value()` | Get current text/value |
| `widget.color(name)` | Set background color (auto text contrast) |
| `widget.font(name, size)` | Set font |
| `widget.pos(x, y, w, h)` | Reposition widget |
| `widget.show()` | Make visible |
| `widget.hide()` | Hide |
| `widget.enable()` | Enable interaction |
| `widget.disable()` | Disable interaction |
| `widget.setTooltip(text)` | Set hover tooltip |
| `widget.style(bg, fg, font, size)` | Combined styling |
| `widget.onClick(cb)` | Click callback |
| `widget.onFocus(cb)` | Focus callback |
| `widget.onBlur(cb)` | Blur callback |
| `widget.onHover(cb)` | Hover callback |
| `widget.onRightClick(cb)` | Right-click callback |
| `widget.onDoubleClick(cb)` | Double-click callback |

---

### Label

Displays static or dynamic text.

```ez
lbl = win.label("Hello", x, y, w, h)
lbl.text("Updated text")
lbl.font("Arial", 18).color("dark")
```

---

### Button

Clickable button with a callback.

```ez
btn = win.button("Save", x, y, w, h, || {
    out "Saved!"
})
btn.color("green")
```

---

### Input

Single-line text field.

```ez
inp = win.input(x, y, w, h)
inp.text("Default value")

value = inp.value()

inp.onTextChange(|| {
    out "Changed: " + inp.value()
})
```

---

### TextArea

Multi-line text editor.

```ez
ta = win.textarea(x, y, w, h)
ta.text("Line one\nLine two")
content = ta.value()

ta.onTextChange(|| {
    out ta.value()
})
```

---

### Checkbox

Toggleable on/off control.

```ez
cb = win.checkbox("Remember me", x, y, w, h)
cb.setChecked(1)

when cb.checked() {
    out "Checked"
}

cb.onChange(|| {
    out "State: " + str(cb.checked())
})
```

---

### Radio

Single-selection radio button. Pass `groupStart = 1` for the first button in a group.

```ez
opt1 = win.radio("Option A", x, y,    w, h, 1)
opt2 = win.radio("Option B", x, y+30, w, h, 0)
opt3 = win.radio("Option C", x, y+60, w, h, 0)

when opt1.selected() {
    out "A selected"
}
```

---

### Dropdown

ComboBox selection list.

```ez
dd = win.dropdown(x, y, w, h)
dd.add("Apple").add("Banana").add("Cherry")

choice = dd.selected()

dd.onChange(|| {
    out "Selected: " + dd.selected()
})
```

---

### Slider

Horizontal range slider.

```ez
sl = win.slider(x, y, w, h, 0, 100)
sl.setValue(50)

current = sl.value()

sl.onChange(|| {
    out "Value: " + str(sl.value())
})
```

---

### ProgressBar

Visual progress indicator.

```ez
pb = win.progressBar(x, y, w, h)
pb.setRange(0, 100)
pb.setValue(75)
```

---

### Spinner

Numeric up/down spinner.

```ez
sp = win.spinner(x, y, w, h, 1, 99)
sp.setValue(25)

age = sp.value()
```

---

### DatePicker

Calendar date picker control.

```ez
dp = win.datePicker(x, y, w, h)
dp.setValue(2025, 6, 15)

date = dp.value()  # Returns "2025-06-15"

dp.onChange(|| {
    out dp.value()
})
```

---

### ListView

Multi-column list with sortable rows.

```ez
lv = win.listView(x, y, w, h)

lv.addColumn("Name", 150)
  .addColumn("Age",  80)
  .addColumn("City", 120)

lv.addRow(["Alice", "25", "Karachi"])
lv.addRow(["Bob",   "30", "Lahore"])

idx  = lv.selected()
row  = lv.getRow(0)  # ["Alice", "25", "Karachi"]

lv.removeRow(1)
lv.clear()

lv.onChange(|| {
    out "Selected row: " + str(lv.selected())
})
```

---

### TreeView

Hierarchical tree structure.

```ez
tv = win.treeView(x, y, w, h)

root   = tv.add(0, "Root")
child1 = tv.add(root, "Child A")
child2 = tv.add(root, "Child B")
        tv.add(child1, "Grandchild")

tv.onChange(|| {
    out "Selected: " + str(tv.selected())
})

tv.clear()
```

---

### Image

Displays an image from file.

```ez
img = win.image("photo.png", x, y, w, h)
img.load("other.jpg")
```

---

### Panel & ScrollPanel

Panels are child containers that can hold any widget.

```ez
panel = win.panel(x, y, w, h)
panel.color("light")
panel.label("Inside panel", 10, 10, 100, 30)
panel.button("Go", 10, 50, 80, 30, || { out "clicked" })
```

For scrollable areas with large content:

```ez
scroll = win.scrollPanel(x, y, w, h)
scroll.contentSize(w, 2000)

repeat i = 0 to 49 {
    scroll.label("Row " + str(i), 10, i * 40, 200, 30)
}
```

---

### Tabs

Tabbed page switcher.

```ez
tabs = win.tabs(x, y, w, h, || {
    out "Tab changed to: " + str(tabs.selected())
})

tabs.add("Home").add("Settings").add("About")
```

---

### Toolbar & StatusBar

```ez
tb = win.toolbar(x, y, w, h)
tb.addButton("New",  || { out "New"  })
tb.addButton("Open", || { out "Open" })
tb.addButton("Save", || { out "Save" })

sb = win.statusbar()
sb.setText("Ready")
```

---

## Layouts

Layouts automatically position widgets — no manual x/y math needed.

### VBox

Stacks widgets vertically with consistent padding.

```ez
vb = win.vbox(10)  # 10px padding

vb.label("Name:", 380, 25)
nameIn = vb.input(380, 35)

vb.label("Email:", 380, 25)
emailIn = vb.input(380, 35)

vb.addSpace(20)

vb.button("Submit", 380, 50, || {
    out nameIn.value()
}).color("blue")
```

**Available VBox methods:**
`label` · `button` · `input` · `textarea` · `dropdown` · `checkbox` · `radio` · `slider` · `progressBar` · `listView` · `treeView` · `datePicker` · `spinner` · `image` · `groupbox` · `separator` · `addSpace`

---

### HBox

Places widgets side by side horizontally.

```ez
hb = win.hbox(10)

hb.label("Sort by:", 80, 30)
hb.dropdown(150, 30)
hb.button("Go", 80, 30, || { out "go" })
```

**Available HBox methods:**
`label` · `button` · `input` · `dropdown` · `checkbox` · `image` · `separator` · `addSpace`

---

### Grid

Arranges widgets in a column grid, wrapping automatically.

```ez
grid = win.grid(3, 10, 10)  # 3 columns, 10px x/y padding
grid.setCellSize(100, 40)

grid.button("1", || { out "1" })
grid.button("2", || { out "2" })
grid.button("3", || { out "3" })
grid.button("4", || { out "4" })  # Wraps to row 2

grid.skipCell()                   # Leave a cell empty
grid.newRow()                     # Force next row
```

For variable column widths:

```ez
grid.setColWidths([200, 120, 80])
```

**Available Grid methods:**
`label` · `button` · `input` · `textarea` · `checkbox` · `dropdown` · `spinner` · `image` · `skipCell` · `newRow`

---

## Styling

### Colors

Apply a named color to any widget. Text contrast is calculated automatically.

```ez
widget.color("blue")
```

| Name | RGB | Auto Text |
|------|-----|-----------|
| `white` | 255, 255, 255 | Dark |
| `black` | 0, 0, 0 | White |
| `gray` | 128, 128, 128 | Dark |
| `dark` | 40, 40, 40 | White |
| `light` | 240, 240, 240 | Dark |
| `red` | 255, 0, 0 | White |
| `green` | 0, 200, 0 | White |
| `blue` | 0, 0, 255 | White |
| `neon` | 0, 255, 255 | Dark |
| `yellow` | 255, 255, 0 | Dark |
| `purple` | 128, 0, 128 | White |
| `orange` | 255, 165, 0 | Dark |

### Fonts

```ez
widget.font("Consolas", 14)
```

### Style Helper

Combines background color, text color, font, and size in one call. Pass `0` to skip any field.

```ez
widget.style("dark", "neon", "Consolas", 14)
widget.style("blue", 0, 0, 0)        # Background only
widget.style(0, 0, "Arial", 16)      # Font only
```

### Themes

Apply a global theme before creating widgets.

```ez
gui.setTheme("neon")

win = gui.window("My App", 600, 400)
```

| Theme | Description |
|-------|-------------|
| `"default"` | Clean, professional |
| `"neon"` | Dark background, cyan accents |
| `"retro"` | Warm brown/orange palette |
| `"ocean"` | Deep blue, light blue accents |

---

## Dialogs

### Alert

```ez
gui.alert("Title", "Message text")
gui.alert("Single-arg message")
```

### Confirm

```ez
result = gui.dialog.confirm("Confirm", "Are you sure?")
when result {
    out "Confirmed"
}
```

### Input Prompt

```ez
name = gui.dialog.input("Enter Name", "What is your name?")
when name {
    out "Hello, " + name
}
```

### File Dialogs

```ez
path = gui.dialog.openFile("Open", "*.txt")
path = gui.dialog.saveFile("Save As", "*.txt")
dir  = gui.dialog.openFolder()
```

### Color Picker

```ez
hex = gui.dialog.colorPicker()  # Returns "#RRGGBB"
when hex {
    out "Picked: " + hex
}
```

---

## Timers

```ez
timerId = win.setTimer(1000, || {  # Every 1000ms
    out "tick"
})

win.killTimer(timerId)
```

---

## Events

Events are registered using `widget.on(event, callback)` or named helpers:

```ez
widget.onClick(cb)
widget.onFocus(cb)
widget.onBlur(cb)
widget.onHover(cb)
widget.onRightClick(cb)
widget.onDoubleClick(cb)

win.onClose(cb)
win.onResize(cb)
win.onKeyPress(cb)
win.onKeyUp(cb)
win.onDrop(cb)
```

`onChange` is available on: `Input`, `TextArea`, `Checkbox`, `Radio`, `Dropdown`, `Slider`, `ListView`, `TreeView`, `DatePicker`

---

## Menus

### Menu Bar

```ez
mb   = win.menubar()
file = mb.addMenu("File")

file.item("New",  || { out "new"  })
file.item("Open", || { out "open" })
file.separator()
file.item("Exit", || { out "exit" })

edit = mb.addMenu("Edit")
edit.item("Undo", || { out "undo" })

sub = edit.submenu("Advanced")
sub.item("Find", || { out "find" })
```

### Context Menu

```ez
ctx = gui.contextMenu()
ctx.item("Copy",   || { out "copy"   })
ctx.item("Paste",  || { out "paste"  })
ctx.separator()
ctx.item("Delete", || { out "delete" })

widget.onRightClick(|| {
    ctx.show(mouseX, mouseY)
})
```

---

## Examples

### Counter

```ez
use "gui"

win = gui.window("Counter", 300, 200)

count = 0
lbl   = win.label("0", 100, 60, 100, 40)
lbl.font("Arial", 28)

win.button("+", 60,  120, 80, 40, || {
    count = count + 1
    lbl.text(str(count))
}).color("green")

win.button("-", 160, 120, 80, 40, || {
    count = count - 1
    lbl.text(str(count))
}).color("red")

win.run()
```

---

### Registration Form

```ez
use "gui"

win = gui.window("Register", 450, 550)
vb  = win.vbox(10)
vb.addSpace(10)

vb.label("Create Account", 430, 40).font("Arial", 22)

vb.label("Full Name:", 430, 24)
nameIn = vb.input(430, 35)

vb.label("Email:", 430, 24)
emailIn = vb.input(430, 35)

vb.label("Country:", 430, 24)
countryDrop = vb.dropdown(430, 35)
countryDrop.add("Pakistan").add("USA").add("UK")

agreeCheck = vb.checkbox("I agree to Terms & Conditions", 430, 30)

vb.addSpace(10)
vb.button("Register", 430, 50, || {
    when not agreeCheck.checked() {
        gui.alert("Error", "Please agree to terms")
        give 0
    }
    gui.alert("Success", "Welcome, " + nameIn.value() + "!")
}).color("blue")

win.run()
```

---

### Todo List

```ez
use "gui"

win = gui.window("Todos", 500, 560)

win.label("New task:", 10, 10, 90, 25)
inp = win.input(105, 10, 285, 25)

win.button("Add", 400, 10, 80, 25, || {
    task = inp.value()
    when len(task) > 0 {
        lv.addRow([task, "Pending"])
        inp.text("")
    }
}).color("green")

lv = win.listView(10, 50, 480, 460)
lv.addColumn("Task",   360)
  .addColumn("Status", 110)

win.button("Done",   10,  520, 100, 30, || {
    i = lv.selected()
    when i >= 0 {
        r = lv.getRow(i)
        lv.removeRow(i)
        lv.addRow([r[0], "Done"])
    }
}).color("blue")

win.button("Delete", 120, 520, 100, 30, || {
    i = lv.selected()
    when i >= 0 { lv.removeRow(i) }
}).color("red")

win.run()
```

---

### Progress Simulation with Timer

```ez
use "gui"

win = gui.window("Progress", 400, 200)
vb  = win.vbox(10)
vb.addSpace(10)

vb.label("Processing…", 380, 25)
pb = vb.progressBar(380, 30)
pb.setRange(0, 100)

prog = 0
timerId = 0

vb.button("Start", 380, 45, || {
    prog    = 0
    timerId = win.setTimer(50, || {
        prog = prog + 1
        pb.setValue(prog)
        when prog >= 100 {
            win.killTimer(timerId)
            gui.alert("Done", "Complete!")
        }
    })
}).color("green")

win.run()
```

---

## Common Issues

**Widget not showing** — Make sure `win.run()` is the last call.

**Can't read input value** — Store the widget in a variable: `inp = win.input(...)`, then `inp.value()`.

**Dropdown too small** — Use a height of 150 or more: `win.dropdown(x, y, w, 150)`.

**Grid cells overlapping** — Call `grid.setCellSize(w, h)` before adding widgets.

**All radio buttons selectable** — Set `groupStart = 1` on the first button in each group only.

---

## Related Libraries

- **db** — SQLite database access
- **pdf** — PDF generation
- **datetime** — Date and time operations

---

*MIT License — Free for any use*