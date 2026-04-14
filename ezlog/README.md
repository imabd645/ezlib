# EZ Log Library v1.0

Professional logging system for EZ applications. Color-coded console output, file logging, log levels, timestamps, and more - all in a clean, simple API.

![Version](https://img.shields.io/badge/version-1.0-blue)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
![Levels](https://img.shields.io/badge/levels-5-green)
![Colors](https://img.shields.io/badge/colors-yes-purple)

## ✨ Features

- 🎨 **Color-Coded Output** - Visual distinction for log levels
- 📊 **5 Log Levels** - DEBUG, INFO, WARN, ERROR, FATAL
- ⏰ **Timestamps** - Precise timing with milliseconds
- 📁 **File Logging** - Write logs to disk
- 🎯 **Level Filtering** - Show only important messages
- 🔧 **Flexible Configuration** - Console/file toggle
- 🚀 **Zero Dependencies** - Pure EZ implementation
- 💾 **Singleton Pattern** - Ready to use instantly

---

## 📦 Installation

```bash
ez install log
```

---

## 🚀 Quick Start

### Basic Logging

```ez
use "log"

log.debug("Application started")
log.info("User logged in")
log.warn("Cache miss")
log.error("Failed to connect to database")
log.fatal("System crash imminent!")
```

**Console Output:**
```
[00:00:05.123] [DEBUG] Application started        (gray)
[00:00:05.456] [INFO] User logged in             (green)
[00:00:05.789] [WARN] Cache miss                 (yellow)
[00:00:06.012] [ERROR] Failed to connect         (red)
[00:00:06.234] [FATAL] System crash imminent!    (red on white)
```

### File Logging

```ez
use "log"

// Log to file
log.setFile("app.log")

log.info("Application started")
log.warn("Low memory")
log.error("Connection timeout")
```

**app.log contents:**
```
[00:00:12.345] [INFO] Application started
[00:00:13.567] [WARN] Low memory
[00:00:14.789] [ERROR] Connection timeout
```

### Set Log Level

```ez
use "log"

// Only show WARN and above (WARN, ERROR, FATAL)
log.setLevel(WARN)

log.debug("Debug message")  // Hidden
log.info("Info message")    // Hidden
log.warn("Warning!")        // Shown ✓
log.error("Error!")         // Shown ✓
```

---

## 📚 API Reference

### Log Levels

Available log levels (in order of severity):

```ez
DEBUG = 0  // Detailed debugging information
INFO  = 1  // General informational messages
WARN  = 2  // Warning messages
ERROR = 3  // Error messages
FATAL = 4  // Critical errors, system failure
```

---

### Logger Methods

#### `log.debug(message)`
Logs debug-level message (gray).

**Use for:** Detailed diagnostic information, variable dumps, trace messages

**Example:**
```ez
log.debug("Request headers: " + str(headers))
log.debug("Entering function processData()")
```

---

#### `log.info(message)`
Logs informational message (green).

**Use for:** Normal application flow, success messages, status updates

**Example:**
```ez
log.info("Server started on port 8080")
log.info("User 'alice' logged in successfully")
log.info("Backup completed: 1.2 GB")
```

---

#### `log.warn(message)`
Logs warning message (yellow).

**Use for:** Unexpected but recoverable situations, deprecated features

**Example:**
```ez
log.warn("API rate limit: 90% consumed")
log.warn("Using deprecated function old_method()")
log.warn("Cache miss - falling back to database")
```

---

#### `log.error(message)`
Logs error message (red).

**Use for:** Errors that need attention but don't crash the app

**Example:**
```ez
log.error("Failed to connect to database: timeout")
log.error("File not found: config.json")
log.error("Payment gateway returned error 500")
```

---

#### `log.fatal(message)`
Logs fatal/critical message (white on red).

**Use for:** Critical errors that may cause shutdown

**Example:**
```ez
log.fatal("Out of memory - cannot continue")
log.fatal("Database corruption detected")
log.fatal("System shutting down")
```

---

### Configuration Methods

#### `log.setLevel(level)`
Sets minimum log level to display.

**Parameters:**
- `level` (number) - One of: DEBUG (0), INFO (1), WARN (2), ERROR (3), FATAL (4)

**Example:**
```ez
// Production: Only show warnings and errors
log.setLevel(WARN)

// Development: Show everything
log.setLevel(DEBUG)

// Critical only
log.setLevel(FATAL)
```

---

#### `log.setFile(filepath)`
Enables file logging to specified path.

**Parameters:**
- `filepath` (string) - Path to log file (created if doesn't exist)

**Example:**
```ez
log.setFile("logs/app.log")
log.setFile("C:/logs/server.log")
log.setFile("debug-" + str(clock()) + ".log")  // Unique filename
```

---

#### `log.enableConsole(boolean)`
Toggles console output.

**Parameters:**
- `boolean` (bool) - `yes` to show, `no` to hide

**Example:**
```ez
// Disable console, only write to file
log.setFile("app.log")
log.enableConsole(no)

log.info("This only goes to file")
```

---

### Custom Logger

Create separate logger instances:

```ez
use "log"

// Default logger
log.info("Main app")

// Custom logger for specific module
dbLogger = Logger()
dbLogger.setLevel(DEBUG)
dbLogger.setFile("database.log")
dbLogger.debug("SQL query executed")

// Custom logger for API
apiLogger = Logger()
apiLogger.setFile("api.log")
apiLogger.info("Request received")
```

---

## 🎯 Complete Examples

### 1. Basic Application Logging

```ez
use "log"

log.info("Application starting...")

// Simulate application flow
log.debug("Initializing configuration")
config = loadConfig()

log.info("Connecting to database")
when not connectDB() {
    log.error("Database connection failed")
    log.warn("Falling back to cache mode")
}

log.info("Server ready on port 8080")

// Simulate error
log.error("Failed to load user profile: user not found")

log.info("Application shutting down")
```

**Output:**
```
[00:00:01.000] [INFO] Application starting...
[00:00:01.100] [DEBUG] Initializing configuration
[00:00:01.200] [INFO] Connecting to database
[00:00:01.300] [ERROR] Database connection failed
[00:00:01.400] [WARN] Falling back to cache mode
[00:00:01.500] [INFO] Server ready on port 8080
[00:00:02.000] [ERROR] Failed to load user profile
[00:00:03.000] [INFO] Application shutting down
```

---

### 2. Production vs Development Logging

```ez
use "log"

// Configuration
isDevelopment = yes

when isDevelopment {
    // Development: Show everything
    log.setLevel(DEBUG)
    log.setFile("debug.log")
    log.info("Running in DEVELOPMENT mode")
} other {
    // Production: Only warnings and errors
    log.setLevel(WARN)
    log.setFile("production.log")
    log.enableConsole(no)  // No console spam
    log.info("Running in PRODUCTION mode")
}

// These only show in development
log.debug("Variable x = " + str(x))
log.debug("Cache size: 1024 bytes")

// These show in both
log.warn("High memory usage: 85%")
log.error("Payment processing failed")
```

---

### 3. Multi-Module Logging

```ez
use "log"

// Main application logger
appLog = Logger()
appLog.setFile("app.log")
appLog.setLevel(INFO)

// Database module logger
dbLog = Logger()
dbLog.setFile("database.log")
dbLog.setLevel(DEBUG)

// API module logger
apiLog = Logger()
apiLog.setFile("api.log")
apiLog.setLevel(INFO)

// Main app
appLog.info("Application started")

// Database operations
dbLog.debug("Connecting to database")
dbLog.debug("Executing query: SELECT * FROM users")
dbLog.info("Query completed: 150 rows")

// API requests
apiLog.info("GET /api/users - 200 OK")
apiLog.warn("POST /api/users - Rate limited")
apiLog.error("GET /api/orders - 500 Server Error")

appLog.info("Application shutdown")
```

---

### 4. Error Tracking

```ez
use "log"

log.setFile("errors.log")

task processPayment(amount, userId) {
    log.info("Processing payment: $" + str(amount))
    
    when amount <= 0 {
        log.error("Invalid amount: " + str(amount))
        give no
    }
    
    when not userExists(userId) {
        log.error("User not found: " + userId)
        give no
    }
    
    // Simulate payment
    success = randomRange(0, 1)
    
    when success {
        log.info("Payment successful: $" + str(amount))
        give yes
    } other {
        log.error("Payment failed: Gateway timeout")
        give no
    }
}

// Process multiple payments
repeat i = 1 to 5 {
    result = processPayment(100 * i, "user" + str(i))
    when not result {
        log.warn("Payment " + str(i) + " failed - retrying...")
    }
}
```

---

### 5. Request/Response Logging

```ez
use "log"

model APILogger {
    init() {
        self.logger = Logger()
        self.logger.setFile("api-requests.log")
    }
    
    task logRequest(method, path, params) {
        msg = method + " " + path
        when params != "" {
            msg = msg + " | Params: " + params
        }
        self.logger.info(msg)
    }
    
    task logResponse(status, duration) {
        msg = "Response: " + str(status) + " (" + str(duration) + "ms)"
        
        when status >= 500 {
            self.logger.error(msg)
        } when status >= 400 {
            self.logger.warn(msg)
        } other {
            self.logger.info(msg)
        }
    }
    
    task logError(error) {
        self.logger.error("Exception: " + error)
    }
}

// Usage
apiLogger = APILogger()

// Request comes in
startTime = clock()
apiLogger.logRequest("POST", "/api/users", "name=Alice")

// Process request...
stop(100)  // Simulate processing

// Response sent
endTime = clock()
duration = endTime - startTime
apiLogger.logResponse(201, duration)
```

**api-requests.log:**
```
[00:00:10.000] [INFO] POST /api/users | Params: name=Alice
[00:00:10.150] [INFO] Response: 201 (150ms)
```

---

### 6. Periodic Status Logging

```ez
use "log"

log.setFile("system-status.log")

model SystemMonitor {
    task logStatus() {
        // Simulate metrics
        cpuUsage = randomRange(20, 90)
        memoryUsage = randomRange(30, 95)
        activeUsers = randomRange(10, 500)
        
        log.info("CPU: " + str(cpuUsage) + "% | Memory: " + str(memoryUsage) + "% | Users: " + str(activeUsers))
        
        // Warnings
        when cpuUsage > 80 {
            log.warn("High CPU usage: " + str(cpuUsage) + "%")
        }
        
        when memoryUsage > 85 {
            log.warn("High memory usage: " + str(memoryUsage) + "%")
        }
        
        when cpuUsage > 95 {
            log.error("Critical CPU usage!")
        }
    }
}

monitor = SystemMonitor()

// Log every 5 seconds
repeat i = 1 to 10 {
    monitor.logStatus()
    stop(5000)
}
```

---

### 7. Debug Mode Toggle

```ez
use "log"

model Application {
    init() {
        self.debugMode = no
        self.setupLogging()
    }
    
    task setupLogging() {
        when self.debugMode {
            log.setLevel(DEBUG)
            log.info("Debug mode ENABLED")
        } other {
            log.setLevel(INFO)
            log.info("Debug mode disabled")
        }
    }
    
    task enableDebug() {
        self.debugMode = yes
        log.setLevel(DEBUG)
        log.info("Debug mode ENABLED")
    }
    
    task disableDebug() {
        self.debugMode = no
        log.setLevel(INFO)
        log.info("Debug mode disabled")
    }
    
    task processData(data) {
        log.debug("Raw data: " + str(data))
        
        // Process...
        processed = data * 2
        
        log.debug("Processed: " + str(processed))
        log.info("Data processed successfully")
        
        give processed
    }
}

app = Application()

// Normal operation - debug hidden
app.processData(10)

// Enable debug
app.enableDebug()
app.processData(20)  // Now shows debug messages
```

---

### 8. Comprehensive Error Logger

```ez
use "log"

model ErrorTracker {
    init() {
        self.errorLog = Logger()
        self.errorLog.setFile("errors.log")
        self.errorLog.setLevel(ERROR)
        self.errorCount = 0
        self.fatalCount = 0
    }
    
    task trackError(category, message, details) {
        self.errorCount = self.errorCount + 1
        
        fullMsg = "[" + category + "] " + message
        when details != "" {
            fullMsg = fullMsg + " | Details: " + details
        }
        
        self.errorLog.error(fullMsg)
        
        when self.errorCount % 10 == 0 {
            self.errorLog.warn("Error count milestone: " + str(self.errorCount))
        }
    }
    
    task trackFatal(message) {
        self.fatalCount = self.fatalCount + 1
        self.errorLog.fatal(message)
        
        // Also log summary
        self.errorLog.fatal("Total errors: " + str(self.errorCount) + " | Fatal: " + str(self.fatalCount))
    }
    
    task getSummary() {
        give {
            errors: self.errorCount,
            fatal: self.fatalCount
        }
    }
}

// Usage
tracker = ErrorTracker()

tracker.trackError("DATABASE", "Connection timeout", "host=db.example.com")
tracker.trackError("API", "Rate limit exceeded", "endpoint=/users")
tracker.trackError("AUTH", "Invalid credentials", "user=alice")

// Critical error
tracker.trackFatal("System out of memory - shutting down")

summary = tracker.getSummary()
log.info("Errors: " + str(summary["errors"]) + " | Fatal: " + str(summary["fatal"]))
```

---

## 🎨 Color Reference

| Level | Color | Code | Use Case |
|-------|-------|------|----------|
| DEBUG | Gray | 8 | Diagnostic information |
| INFO | Green | 10 | Normal operations |
| WARN | Yellow | 14 | Warnings, deprecations |
| ERROR | Red | 12 | Errors, failures |
| FATAL | White on Red | 79 | Critical system errors |

---

## 💡 Best Practices

### 1. Use Appropriate Levels

```ez
// ✅ GOOD - Correct level usage
log.debug("Variable x = " + str(x))        // Debug info
log.info("User logged in")                 // Normal event
log.warn("Cache miss - using fallback")    // Recoverable issue
log.error("Failed to save file")           // Error needs attention
log.fatal("Database corrupted")            // Critical failure

// ❌ BAD - Wrong levels
log.error("User logged in")                // Not an error!
log.info("System crash")                   // Should be FATAL!
```

---

### 2. Set Appropriate Levels for Environment

```ez
// ✅ GOOD - Environment-specific
when isProduction {
    log.setLevel(WARN)  // Only warnings and errors
} other {
    log.setLevel(DEBUG)  // Everything in development
}

// ❌ BAD - Same level everywhere
log.setLevel(DEBUG)  // Too much noise in production!
```

---

### 3. Include Context in Messages

```ez
// ✅ GOOD - Detailed, actionable
log.error("Failed to connect to database: timeout after 30s (host: db.example.com, port: 5432)")

// ❌ BAD - Vague
log.error("Connection failed")
```

---

### 4. Use Separate Log Files for Modules

```ez
// ✅ GOOD - Separate concerns
dbLogger = Logger()
dbLogger.setFile("database.log")

apiLogger = Logger()
apiLogger.setFile("api.log")

// ❌ BAD - Everything mixed
log.setFile("everything.log")  // Hard to debug!
```

---

### 5. Don't Log Sensitive Data

```ez
// ❌ BAD - Security risk!
log.debug("Password: " + password)
log.info("Credit card: " + cardNumber)

// ✅ GOOD - Redacted or hashed
log.debug("Password length: " + str(len(password)))
log.info("Payment processed for card ending: ****" + substr(cardNumber, -4, 4))
```

---

## 🐛 Common Issues

### Logs Not Appearing in File
**Cause:** File path doesn't exist  
**Solution:** Create directory first or use existing path

```ez
// ✅ GOOD
fs.createDir("logs")
log.setFile("logs/app.log")

// ❌ BAD
log.setFile("nonexistent/folder/app.log")  // Fails!
```

---

### Too Much Debug Output
**Cause:** Log level too low  
**Solution:** Increase log level

```ez
// Production
log.setLevel(WARN)  // Only WARN, ERROR, FATAL
```

---

### Colors Not Showing
**Cause:** Terminal doesn't support colors  
**Solution:** Normal - timestamps and levels still shown

---

## 📊 Performance Tips

1. **Set appropriate log level** - DEBUG generates lots of messages
2. **Use file logging sparingly** - Disk I/O can be slow
3. **Avoid logging in tight loops** - Log summary instead
4. **Disable console in production** - File only is faster

```ez
// ✅ GOOD - Log summary
repeat i = 1 to 1000 {
    process(i)
}
log.info("Processed 1000 items")

// ❌ BAD - Logs 1000 times!
repeat i = 1 to 1000 {
    log.debug("Processing item " + str(i))
    process(i)
}
```

---

## 🔗 Related Libraries

- **fs** - File system operations (used internally)
- **datetime** - Add date to log filenames
- **db** - Store logs in database


---

## 📄 License

MIT License - Free for any use

---

## 🤝 Contributing

Found a bug? Want a feature?  
Contributions welcome!

---

**Made with ❤️ for the EZ Language Community**

*Professional logging made simple* 🚀
