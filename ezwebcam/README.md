# EZ Webcam

Access your PC's physical webcam and capture photos using pure EZ FFI.

## Installation
```javascript
use "webcam"
```

## Creating a Webcam Viewer

```javascript
# Open the webcam stream in a new native window
success = webcam.open("Live Feed", 640, 480)

# Keep the window refreshing smoothly
while true {
    webcam.update()
    wait(16)
}

# Capture a photo!
webcam.capture("selfie.bmp")

# Close safely
webcam.close()
```
