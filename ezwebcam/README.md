# EZ Webcam Library (`ezwebcam`)

A high-performance wrapper around Windows Video for Windows (VFW) / AVICap32. This library allows you to open a live webcam feed, take BMP photos, record AVI videos, copy frames to the clipboard, and trigger hardware configuration dialogs natively.

## Installation

```bash
ez install ezwebcam 1.0.0
```

## Features
- **Live Preview Window**: Embeds the native hardware preview stream.
- **Photo Capture**: Capture the current frame to a BMP file.
- **Video Recording**: Record the stream directly to an AVI file on disk.
- **Clipboard Integration**: Instantly copy the current frame to the Windows clipboard.
- **Hardware Dialogs**: Open your webcam's driver properties (resolution, brightness, format).

## Basic Usage

```ez
use "ezwebcam"

# 1. Initialize the webcam window (Title, Width, Height)
cam = Webcam("My Live Camera", 640, 480)

# 2. Add an event loop to keep the window updating
while true {
    cam.update()
    
    # You could check for keypresses here to trigger captures!
    wait(16) # Render at ~60 FPS
}

# 3. Clean up when finished
cam.close()
```

## API Reference

### `.capturePhoto(filepath)`
Saves the current frame to the specified file (must be `.bmp`).
```ez
cam.capturePhoto("selfie.bmp")
```

### `.startRecording(filepath)` / `.stopRecording()`
Records an AVI video stream to the disk.
```ez
cam.startRecording("video.avi")
# ... wait ...
cam.stopRecording()
```

### `.copyToClipboard()`
Copies the current frame to the Windows clipboard.
```ez
cam.copyToClipboard()
```

### `.showFormatDialog()` / `.showSourceDialog()`
Opens the native Windows hardware dialogs. This is extremely useful for changing the camera's resolution, frame rate, or adjusting brightness and contrast.
```ez
cam.showFormatDialog()
```
