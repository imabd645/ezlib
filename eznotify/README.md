# EZ Notify

A simple library for displaying native Windows dialog boxes and prompts using FFI (`MessageBoxA`).

## Installation
```javascript
use "notify"
```

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

