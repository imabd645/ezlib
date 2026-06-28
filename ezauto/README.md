# ezauto

The `ezauto` package provides native Win32 FFI desktop automation for keyboard and mouse control on Windows. It is designed as a lightweight clone of PyAutoGUI for the EZ programming language!

## Keyboard Automation
```ez
use "ezauto"

# Type a string automatically (handles shift keys seamlessly)
ezauto.type("Hello World!")

# Press single keys
ezauto.press("enter")
ezauto.down("shift")
ezauto.up("shift")

# Press multiple keys at once (e.g., Ctrl+C)
ezauto.hotkey(["ctrl", "c"])

# Check if a key is currently physically pressed down
is_held = ezauto.is_down("ctrl")
```

## Mouse Automation
```ez
use "auto" as ezauto

# Move the mouse
ezauto.move(500, 500)

# Move the mouse relative to its current position
ezauto.move_rel(50, -50)

# Clicking
ezauto.click("left")
ezauto.double_click("left")

# Drag and drop
ezauto.drag(100, 100, "left")

# Scroll wheel
ezauto.scroll(100) # Scroll up

# Get current mouse coordinates
pos = ezauto.pos()
out pos["x"], pos["y"]
```

## Screen & Utilities
```ez
use "ezauto"

# Native precision delay (milliseconds)
ezauto.delay(500)

# Get the RGB color of a specific pixel on the screen!
color = ezauto.get_pixel(500, 500)
out "R: " + str(color["r"]) + " G: " + str(color["g"]) + " B: " + str(color["b"])

# Get screen resolution
sz = ezauto.size()
out sz["width"], sz["height"]
```
