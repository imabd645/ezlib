# ezgui — GUI Desktop Application Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezgui"`  
> **File:** `E:\ezlib\ezgui\main.ez`  
> **Requires:** EZ GUI runtime built-ins

---

## Overview

`ezgui` is a high-level, object-oriented desktop GUI library for EZ, built on native EZ GUI built-ins. It provides:

- **Window and widget creation** — labels, buttons, inputs, dropdowns, checkboxes, etc.
- **Layout managers** — `VBox` (vertical stack), `HBox` (horizontal stack), `Grid`
- **Menus** — menu bar, context menus, submenus
- **Advanced widgets** — ListView, TreeView, DatePicker, Spinner, ProgressBar, Tabs
- **Window management** — resize, center, opacity, always-on-top, drag-and-drop
- **Themes** — neon, retro, ocean
- **Dialogs** — alert, confirm, input, file open/save, color picker, font picker
- **System tray** integration
- **Notification/balloon** messages

---

## Quick Start

```ez
use "ezgui"

win = gui.window("My App", 400, 300)

win.label("Hello, World!", 10, 10, 200, 30)

btn = win.button("Click Me", 10, 60, 120, 35, || {
    gui.alert("You clicked the button!")
})

win.show()
win.run()
```

---

## `gui` Object

After `use "ezgui"`, a `gui` global dictionary provides the main entry points.

### `gui.window(title, width, height)` → `Window`
Creates a new top-level window.

```ez
use "ezgui"

win = gui.window("My Application", 800, 600)
win.show()
win.run()
```

### `gui.alert(title, message)`
Shows a modal alert dialog. If called with one argument, shows `"EZ"` as title.

```ez
gui.alert("Warning", "File not found!")
gui.alert("Short alert message")  # Title = "EZ"
```

### `gui.setTheme(name)`
Sets the global color theme. Available themes:
- `"neon"` — Dark background with cyan accents
- `"retro"` — Warm browns with orange accents
- `"ocean"` — Deep blue with blue accents

```ez
use "ezgui"

gui.setTheme("neon")
win = gui.window("Neon App", 600, 400)
```

### `gui.contextMenu()` → `ContextMenu`
Creates a new context (right-click) menu.

---

## Model: `Window`

Extends `ContainerWidget`. Creates a top-level native window.

### `Window(title, width, height)`
```ez
use "ezgui"

win = Window("My Window", 600, 400)
```

Or use the shorthand:
```ez
win = gui.window("My Window", 600, 400)
```

---

## Window Management

```ez
use "ezgui"

win = gui.window("Demo", 600, 400)

win.center()                 # Center on screen
win.setTitle("New Title")   # Change title
win.resize(800, 600)         # Resize window
win.setOpacity(200)          # 0–255 transparency
win.setAlwaysOnTop(true)    # Float above other windows
win.setMinSize(300, 200)    # Set minimum resize boundary
win.setIcon("app.ico")      # Set window icon (ICO file)
win.minimize()               # Minimize to taskbar
win.maximize()               # Maximize
win.restore()                # Restore from min/max

size = win.getSize()         # → [width, height]
pos = win.getPos()           # → [x, y]
```

---

## Window Events

```ez
use "ezgui"

win = gui.window("Events Demo", 400, 300)

win.onClose(|| {
    out "Window closing!"
})

win.onResize(|| {
    size = win.getSize()
    out "Resized to " + str(size[0]) + "x" + str(size[1])
})

win.onKeyPress(|key| {
    out "Key pressed: " + str(key)
})

win.onKeyUp(|key| {
    out "Key released: " + str(key)
})

win.onDrop(|files| {
    out "Files dropped: " + str(files)
})

win.show()
win.run()
```

---

## Timers

```ez
use "ezgui"

win = gui.window("Timer Demo", 300, 200)
count = 0

timerId = win.setTimer(1000, || {
    count = count + 1
    out "Tick: " + str(count)
    when count >= 10 { win.killTimer(timerId) }
})

win.show()
win.run()
```

---

## Widget Reference

All widgets are created via factory methods on `Window` or `Panel`. Widgets extend `Widget` (which extends `ContainerWidget` for containers).

### Common Widget Methods (from `Widget`)

| Method | Description |
|---|---|
| `widget.text(v)` | Set text/value |
| `widget.value()` | Get text/value |
| `widget.getText()` | Alias for `value()` |
| `widget.setText(v)` | Alias for `text(v)` |
| `widget.show()` | Show widget |
| `widget.hide()` | Hide widget |
| `widget.enable()` | Enable widget |
| `widget.disable()` | Disable widget |
| `widget.color(name)` | Set background color (auto-contrast text) |
| `widget.font(name, size)` | Set font |
| `widget.pos(x, y, w, h)` | Set position and size |
| `widget.setTooltip(text)` | Set hover tooltip |
| `widget.onClick(cb)` | Click handler (legacy) |
| `widget.on(event, cb)` | Generic event handler |
| `widget.onFocus(cb)` | Focus event |
| `widget.onBlur(cb)` | Blur event |
| `widget.onRightClick(cb)` | Right-click event |
| `widget.onDoubleClick(cb)` | Double-click event |
| `widget.onHover(cb)` | Hover event |
| `widget.style(bg, fg, font, size)` | Combined style setter |

---

## Widget Factory Methods

All of these are available on `Window`, `Panel`, and layout managers.

### `win.label(text, x, y, w, h)` → `Label`
```ez
lbl = win.label("Name:", 10, 10, 80, 25)
lbl.color("dark")
lbl.font("Arial", 12)
```

### `win.button(text, x, y, w, h, callback)` → `Button`
```ez
btn = win.button("Submit", 10, 50, 100, 35, || {
    out "Submitted!"
})
btn.color("blue")
btn.click(|| { out "Alternative click handler" })
```

### `win.input(x, y, w, h)` → `Input`
```ez
field = win.input(10, 90, 200, 28)
field.text("Default value")
field.onTextChange(|| {
    out "Input changed: " + field.value()
})
val = field.value()
```

### `win.textarea(x, y, w, h)` → `TextArea`
```ez
ta = win.textarea(10, 130, 300, 150)
ta.text("Initial content\nLine 2")
ta.onTextChange(|| {
    out "Content: " + ta.value()
})
```

### `win.dropdown(x, y, w, h)` → `Dropdown`
```ez
drp = win.dropdown(10, 290, 200, 150)
drp.add("Option 1")
drp.add("Option 2")
drp.add("Option 3")
drp.onChange(|| {
    out "Selected: " + str(drp.selected())
})
```

### `win.checkbox(text, x, y, w, h)` → `Checkbox`
```ez
chk = win.checkbox("Enable notifications", 10, 330, 200, 25)
chk.setChecked(true)
chk.onChange(|| {
    out "Checked: " + str(chk.checked())
})
```

### `win.radio(text, x, y, w, h, groupStart)` → `Radio`
```ez
# groupStart = true for first radio in group, false/0 for others
r1 = win.radio("Option A", 10, 360, 100, 25, true)
r2 = win.radio("Option B", 10, 390, 100, 25, false)
r3 = win.radio("Option C", 10, 420, 100, 25, false)

r1.onChange(|| { out "A selected: " + str(r1.selected()) })
```

### `win.slider(x, y, w, h, min, max)` → `Slider`
```ez
slider = win.slider(10, 450, 300, 25, 0, 100)
slider.setValue(50)
slider.onChange(|| {
    out "Value: " + str(slider.value())
})
```

### `win.progressBar(x, y, w, h)` → `ProgressBar`
```ez
pb = win.progressBar(10, 490, 300, 20)
pb.setRange(0, 100)
pb.setValue(75)
```

### `win.listView(x, y, w, h)` → `ListView`
```ez
lv = win.listView(10, 10, 400, 200)
lv.addColumn("Name", 150)
lv.addColumn("Age", 100)
lv.addColumn("Email", 150)

lv.addRow(["Alice", "30", "alice@example.com"])
lv.addRow(["Bob", "25", "bob@example.com"])

lv.onChange(|| {
    idx = lv.selected()
    row = lv.getRow(idx)
    out "Selected: " + str(row)
})

lv.removeRow(0)  # Remove first row
lv.clear()       # Remove all rows
```

### `win.treeView(x, y, w, h)` → `TreeView`
```ez
tv = win.treeView(10, 230, 200, 200)

root = tv.add(0, "Root")
child1 = tv.add(root, "Child 1")
child2 = tv.add(root, "Child 2")
tv.add(child1, "Leaf 1")

tv.onChange(|| {
    sel = tv.selected()
    out "Selected: " + str(sel)
})
```

### `win.datePicker(x, y, w, h)` → `DatePicker`
```ez
dp = win.datePicker(10, 450, 200, 28)
dp.setValue(2025, 6, 11)  # Year, Month, Day
dp.onChange(|| {
    date = dp.value()
    out "Date: " + str(date)
})
```

### `win.spinner(x, y, w, h, min, max)` → `Spinner`
```ez
sp = win.spinner(10, 490, 100, 28, 1, 100)
sp.setValue(42)
out sp.value()  # → 42
```

### `win.image(path, x, y, w, h)` → `Image`
```ez
img = win.image("logo.png", 10, 10, 200, 100)
img.load("other_image.png")  # Change image
```

### `win.panel(x, y, w, h)` → `Panel`
A child container that can hold its own widgets.
```ez
pnl = win.panel(10, 10, 300, 200)
pnl.label("Inside panel", 5, 5, 150, 25)
pnl.button("Panel Button", 5, 40, 100, 30, || { out "Panel button!" })
```

### `win.scrollPanel(x, y, w, h)` → `ScrollPanel`
A scrollable container.
```ez
sp = win.scrollPanel(10, 10, 300, 200)
sp.contentSize(600, 800)  # Virtual content size (scrollable area)
sp.label("Far right", 550, 10, 100, 25)
sp.label("Far down", 10, 750, 100, 25)
```

### `win.groupbox(text, x, y, w, h)` → `GroupBox`
A labeled group frame.
```ez
gb = win.groupbox("Settings", 10, 10, 250, 150)
```

### `win.separator(x, y, w, h)` → `Separator`
A horizontal/vertical line divider.
```ez
sep = win.separator(10, 100, 380, 2)  # Horizontal divider
```

---

## Tabs

```ez
use "ezgui"

win = gui.window("Tabbed App", 600, 400)

tabs = win.tabs(0, 0, 600, 400, || {
    out "Tab changed to: " + str(tabs.selected())
})

tabs.add("General")
tabs.add("Advanced")
tabs.add("About")

win.show()
win.run()
```

---

## Menus

### Menu Bar
```ez
use "ezgui"

win = gui.window("Menus Demo", 600, 400)
bar = win.menubar()

fileMenu = bar.addMenu("File")
fileMenu.item("New", || { out "New file" })
fileMenu.item("Open...", || { out "Opening..." })
fileMenu.separator()
fileMenu.item("Exit", || { out "Exiting..." })

editMenu = bar.addMenu("Edit")
editMenu.item("Cut", || { out "Cut" })
editMenu.item("Copy", || { out "Copy" })
editMenu.item("Paste", || { out "Paste" })

# Submenu
toolsMenu = bar.addMenu("Tools")
extMenu = toolsMenu.submenu("Extensions")
extMenu.item("Plugin Manager", || { out "Plugin Manager" })

win.show()
win.run()
```

### Context Menu
```ez
use "ezgui"

win = gui.window("Right-Click Demo", 400, 300)

ctxMenu = gui.contextMenu()
ctxMenu.item("Copy", || { out "Copy" })
ctxMenu.item("Paste", || { out "Paste" })
ctxMenu.separator()
ctxMenu.item("Delete", || { out "Delete" })

win.onRightClick(|x, y| {
    ctxMenu.show(x, y)
})

win.show()
win.run()
```

---

## Layout Managers

### VBox — Vertical Stack

```ez
use "ezgui"

win = gui.window("VBox Demo", 300, 400)

vb = win.vbox(10)   # padding = 10

lbl  = vb.label("Username:", 200, 25)
name = vb.input(200, 28)
lbl2 = vb.label("Password:", 200, 25)
pass = vb.input(200, 28)
vb.addSpace(10)
btn  = vb.button("Login", 200, 40, || {
    out "Login: " + name.value()
})

win.show()
win.run()
```

### HBox — Horizontal Stack

```ez
use "ezgui"

win = gui.window("HBox Demo", 600, 100)

hb = win.hbox(10)

hb.button("File", 80, 35, || { out "File" })
hb.button("Edit", 80, 35, || { out "Edit" })
hb.button("View", 80, 35, || { out "View" })
hb.separator(35)
hb.button("Help", 80, 35, || { out "Help" })

win.show()
win.run()
```

### Grid — Column Layout

```ez
use "ezgui"

win = gui.window("Grid Demo", 500, 400)

g = win.grid(2, 10, 10)    # 2 columns, 10px padding each axis
g.setCellSize(220, 30)

g.label("First Name")
inp1 = g.input()

g.label("Last Name")
inp2 = g.input()

g.label("Email")
inp3 = g.input()

g.label("Age")
sp = g.spinner(18, 120)

g.skipCell()    # Empty cell
g.button("Submit", || {
    out "Submitting..."
})

win.show()
win.run()
```

---

## Dialogs

```ez
use "ezgui"

win = gui.window("Dialogs", 400, 300)

# Confirm dialog
win.button("Confirm?", 10, 10, 150, 35, || {
    result = gui.dialog.confirm("Confirm", "Are you sure?")
    out result   # → true/false
})

# Input dialog
win.button("Get Name", 10, 60, 150, 35, || {
    name = gui.dialog.input("Name", "Enter your name:")
    out "Name: " + name
})

# Open file dialog
win.button("Open File", 10, 110, 150, 35, || {
    path = gui.dialog.openFile("Open", "*.ez;*.txt")
    out "Path: " + path
})

# Save file dialog
win.button("Save File", 10, 160, 150, 35, || {
    path = gui.dialog.saveFile("Save As", "*.txt")
    out "Save to: " + path
})

# Color picker
win.button("Pick Color", 10, 210, 150, 35, || {
    color = gui.dialog.colorPicker()
    out "Color: " + str(color)
})

# Font picker
win.button("Pick Font", 10, 260, 150, 35, || {
    font = gui.dialog.fontPicker()
    out "Font: " + str(font)
})

win.show()
win.run()
```

---

## Colors

The `guiColors` map is exported globally. Use color names with `widget.color(name)`:

| Name | RGB |
|---|---|
| `"white"` | 255, 255, 255 |
| `"black"` | 0, 0, 0 |
| `"gray"` | 128, 128, 128 |
| `"dark"` | 40, 40, 40 |
| `"light"` | 240, 240, 240 |
| `"red"` | 255, 0, 0 |
| `"green"` | 0, 200, 0 |
| `"blue"` | 0, 0, 255 |
| `"neon"` | 0, 255, 255 |
| `"yellow"` | 255, 255, 0 |
| `"purple"` | 128, 0, 128 |
| `"orange"` | 255, 165, 0 |

`widget.color()` auto-selects white or dark text based on background luminance.

---

## Full Example: Calculator

```ez
use "ezgui"

win = gui.window("Calculator", 300, 400)
display = ""

vb = win.vbox(5)

screen = vb.label("0", 280, 50)
screen.color("dark")
screen.font("Consolas", 24)

digits = [
    ["7", "8", "9", "/"],
    ["4", "5", "6", "*"],
    ["1", "2", "3", "-"],
    ["0", ".", "=", "+"]
]

get row in digits {
    hb = win.hbox(5)
    hb.addSpace(5)
    get key in row {
        btn = hb.button(key, 60, 55, || {})
        btn.color("gray")
    }
}

vb.button("C", 280, 40, || {
    display = ""
    screen.text("0")
})

win.show()
win.run()
```

---

## Edge Cases & Important Notes

### Threading
All GUI operations must happen on the main thread. Do not call `win.label()`, `win.button()`, etc. from spawned tasks. Use timers (`win.setTimer()`) for background updates to the GUI.

### `win.run()` Blocks
`win.run()` enters the Windows message loop and blocks until the window is closed. Put all setup code before `win.run()`.

### Dropdown Height
When creating dropdowns manually, set the height large enough (e.g. `150`) to allow the dropdown list to expand. VBox's `dropdown()` automatically sets a minimum height of 150 if the given height is less.

### `groupStart` in Radio Buttons
Pass `true` (or `1`) as the last argument to the first radio button in a group. All subsequent radios in the same group should pass `false` (or `0`). This sets the `WS_GROUP` style correctly.

### Widget Callbacks are Closures
Callbacks capture variables by reference. If you need to pass state to a callback, close over it:
```ez
count = 0
btn = win.button("Count", 10, 10, 100, 30, || {
    count = count + 1
    out count
})
```

---

*Documentation generated from `E:\ezlib\ezgui\main.ez` — EZ GUI Library*