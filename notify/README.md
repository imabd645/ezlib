# EZ Notify

A simple library for displaying native Windows dialog boxes and prompts using FFI (`MessageBoxA`).

## API Reference

### `alert(message, title = "EZ Notification")`
Shows a standard informational alert box.

### `popup(message, title = "EZ Notification", timeoutMs = 3000)`
Shows a non-blocking informational popup that automatically disappears after the specified number of milliseconds! Utilizes the undocumented Windows `MessageBoxTimeoutA` API to achieve this seamlessly.

### `tray(message, title = "EZ Notification", timeoutMs = 3000)`
Shows a standard Windows System Tray (balloon) notification. This is achieved by transparently constructing a background PowerShell command via the `WinExec` FFI API, ensuring your EZ script continues executing instantly without blocking.

### `error(message, title = "Error")`
Shows a standard error dialog box.

## Usage

### Information & Alerts
```javascript
notify.alert("Task completed successfully!")
notify.warn("Disk space is running low.")
notify.error("Failed to connect to the database.", "Connection Error")
```

### User Input Prompts
```javascript
# confirm() gives a Yes/No dialog
is_sure = notify.confirm("Are you sure you want to delete this?")
when is_sure {
    print("User clicked Yes!")
}

# prompt() gives an OK/Cancel dialog
proceed = notify.prompt("Please insert the USB drive and click OK.")
when proceed {
    print("User clicked OK!")
}
```

