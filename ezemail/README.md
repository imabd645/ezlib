# ezemail — SMTP Email Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezemail"`  
> **File:** `E:\ezlib\ezemail\main.ez`  
> **Requires:** `libcurl-4.dll`, `msvcrt.dll`

---

## Overview

`ezemail` provides a full SMTP email client for EZ, built on top of `libcurl` through FFI. It supports:

- Plain-text and HTML emails
- SSL/TLS (port 465 / STARTTLS on 587)
- Gmail App Passwords and any custom SMTP provider
- Reply-To headers
- Message priority
- Custom headers

---

## Quick Start

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "you@gmail.com", "your-app-password")
client.send("you@gmail.com", "friend@example.com", "Hello!", "This is a test email.")
```

---

## Model: `SmtpClient`

### `SmtpClient(server, port, username, password)`

Creates an SMTP client instance.

**Parameters:**
- `server` — SMTP server hostname (e.g. `"smtp.gmail.com"`)
- `port` — Port number. Use `465` for SMTPS (implicit SSL), `587` for STARTTLS, `25` for plain.
- `username` — Your email username / login.
- `password` — Your SMTP password or App Password.

```ez
use "ezemail"

# Gmail
client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "abcd efgh ijkl mnop")

# Outlook / Hotmail
client = SmtpClient("smtp-mail.outlook.com", 587, "me@outlook.com", "mypassword")

# Custom SMTP
client = SmtpClient("mail.myserver.com", 465, "user@myserver.com", "pass")
```

---

### `SmtpClient` Properties

| Property | Default | Description |
|---|---|---|
| `useSsl` | `true` | Enable SSL/TLS (`CURLUSESSL_ALL`) |
| `verbose` | `false` | Enable libcurl debug output |
| `isHtml` | `false` | Set content type to `text/html` |
| `replyTo` | `""` | Reply-To header value |
| `priority` | `""` | X-Priority: `"1"` (High), `"3"` (Normal), `"5"` (Low) |
| `customHeaders` | `{}` | Extra headers dictionary |

Set these after construction:
```ez
client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "apppass")
client.isHtml = true
client.replyTo = "noreply@myapp.com"
client.priority = "1"
```

---

### `client.send(fromEmail, toEmail, subject, body)` → `true`

Sends an email.

**Parameters:**
- `fromEmail` — Sender email address string.
- `toEmail` — Recipient email address string.
- `subject` — Subject line string.
- `body` — Email body. If `client.isHtml = true`, this should be valid HTML.

**Returns:** `true` on success.

**Throws:** String error with the curl error code on failure.

---

## Usage Examples

### Plain Text Email

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "sender@gmail.com", "app-password-here")

result = client.send(
    "sender@gmail.com",
    "recipient@example.com",
    "Test Email from EZ",
    "Hello! This email was sent from an EZ program."
)

when result { out "Email sent successfully!" }
```

---

### HTML Email

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "sender@gmail.com", "app-password-here")
client.isHtml = true

html = "<h1>Hello from EZ!</h1>"
html = html + "<p>This is an <strong>HTML</strong> email.</p>"
html = html + "<p style='color:blue;'>Sent with ezemail library.</p>"

client.send("sender@gmail.com", "recipient@example.com", "HTML Test", html)
```

---

### High-Priority Email

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "alerts@myapp.com", "apppass")
client.priority = "1"  # High priority

client.send(
    "alerts@myapp.com",
    "admin@myapp.com",
    "URGENT: Server Down",
    "The production server is not responding. Immediate action required."
)
```

---

### Email with Reply-To

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "noreply@myapp.com", "apppass")
client.replyTo = "support@myapp.com"

client.send(
    "noreply@myapp.com",
    "user@example.com",
    "Your Order Confirmation",
    "Thank you for your order! Reply to this email for support."
)
```

---

### Custom Headers

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "apppass")
client.customHeaders = {
    "X-Mailer": "EZ Email System v1.0",
    "X-Campaign-ID": "summer2025",
    "List-Unsubscribe": "<mailto:unsubscribe@myapp.com>"
}

client.send("me@gmail.com", "user@example.com", "Summer Sale!", "Big deals inside!")
```

---

### Debug Mode (Verbose)

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "apppass")
client.verbose = true  # Prints full curl handshake to stdout

client.send("me@gmail.com", "me@gmail.com", "Debug Test", "Testing SMTP connection")
```

---

## SMTP Provider Settings

| Provider | Server | Port | SSL | Notes |
|---|---|---|---|---|
| Gmail | `smtp.gmail.com` | `587` | STARTTLS | Requires App Password with 2FA |
| Gmail (direct SSL) | `smtp.gmail.com` | `465` | SMTPS | Use port 465 for implicit SSL |
| Outlook/Hotmail | `smtp-mail.outlook.com` | `587` | STARTTLS | Use account password |
| Yahoo | `smtp.mail.yahoo.com` | `587` | STARTTLS | Requires App Password |
| SendGrid | `smtp.sendgrid.net` | `587` | STARTTLS | Use API key as password |
| Mailgun | `smtp.mailgun.org` | `587` | STARTTLS | Use domain SMTP credentials |

---

## Gmail App Password Setup

1. Enable 2-Step Verification on your Google Account.
2. Go to **Google Account → Security → 2-Step Verification → App passwords**.
3. Generate a new App Password for "Mail" / "Windows Computer".
4. Use the 16-character code (without spaces) as your password.

```ez
# Example with App Password
client = SmtpClient("smtp.gmail.com", 587, "you@gmail.com", "abcdefghijklmnop")
```

---

## Edge Cases & Important Notes

### Port 465 vs 587
- **Port 465 (SMTPS):** Uses implicit SSL from the start. The library sends `smtps://` URL.
- **Port 587 (STARTTLS):** Starts plain, upgrades to SSL. The library sends `smtp://` URL with `CURLUSESSL_ALL`.

```ez
# SMTPS (465) — implicit SSL
client = SmtpClient("smtp.gmail.com", 465, "me@gmail.com", "pass")

# STARTTLS (587) — upgrade
client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "pass")
```

### Certificate Validation Disabled
SSL certificate validation is **disabled** (`CURLOPT_SSL_VERIFYPEER = 0`, `CURLOPT_SSL_VERIFYHOST = 0`) for maximum compatibility. This means man-in-the-middle attacks are possible. Do not use for high-security, production environments without re-enabling certificate verification via the libcurl FFI.

### Temporary File Creation
The library writes the email payload to a temporary file named `temp_email_payload.txt` in the current working directory before sending. This file is not automatically deleted after sending.

```ez
# temp_email_payload.txt will be created in the current directory.
# Clean it up manually if needed:
use "ezfs"
fs.remove("temp_email_payload.txt")
```

### Single Recipient Only
The current implementation supports one recipient per `send()` call. To send to multiple recipients, call `send()` multiple times:

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "pass")
recipients = ["a@example.com", "b@example.com", "c@example.com"]

get addr in recipients {
    client.send("me@gmail.com", addr, "Newsletter", "Hello!")
}
```

### Error Handling
Wrap `send()` in a `try/catch` block:

```ez
use "ezemail"

client = SmtpClient("smtp.gmail.com", 587, "me@gmail.com", "pass")

try {
    client.send("me@gmail.com", "user@example.com", "Test", "Body")
    out "Sent!"
} catch e {
    out "Failed: " + e
}
```

### libcurl-4.dll Not Found
If `libcurl-4.dll` is not in the system PATH, the library throws immediately on import:
```
Error: libcurl-4.dll not found.
```
Ensure the DLL is in the same directory as your EZ script or in `C:\Windows\System32`.

---

## Full Example: Welcome Email System

```ez
use "ezemail"

SMTP_SERVER = "smtp.gmail.com"
SMTP_PORT = 587
FROM_EMAIL = "noreply@myapp.com"
APP_PASS = "your-app-password"

task sendWelcomeEmail(userName, userEmail) {
    client = SmtpClient(SMTP_SERVER, SMTP_PORT, FROM_EMAIL, APP_PASS)
    client.isHtml = true
    client.replyTo = "support@myapp.com"
    
    subject = "Welcome to MyApp, " + userName + "!"
    
    body = "<html><body>"
    body = body + "<h1>Welcome, " + userName + "!</h1>"
    body = body + "<p>Thank you for joining MyApp.</p>"
    body = body + "<p>Get started by <a href='https://myapp.com/dashboard'>visiting your dashboard</a>.</p>"
    body = body + "<hr><small>MyApp Team | Unsubscribe</small>"
    body = body + "</body></html>"
    
    try {
        client.send(FROM_EMAIL, userEmail, subject, body)
        out "Welcome email sent to " + userEmail
        give true
    } catch e {
        out "Failed to send email: " + e
        give false
    }
}

sendWelcomeEmail("Alice", "alice@example.com")
sendWelcomeEmail("Bob", "bob@example.com")
```

---

## Full Example: Alert System

```ez
use "ezemail"

ALERT_EMAIL = "alerts@company.com"
ADMIN_EMAIL = "admin@company.com"

task sendAlert(title, details, level) {
    client = SmtpClient("smtp.gmail.com", 587, ALERT_EMAIL, "app-pass")
    client.isHtml = true
    
    when level == "critical" { client.priority = "1" }
    when level == "warning" { client.priority = "3" }
    when level == "info" { client.priority = "5" }
    
    colors = {"critical": "#dc3545", "warning": "#ffc107", "info": "#17a2b8"}
    bg = "#dc3545"
    when has_key(colors, level) { bg = colors[level] }
    
    body = "<div style='font-family:Arial;'>"
    body = body + "<div style='background:" + bg + ";color:white;padding:12px;'>"
    body = body + "<strong>" + upper(level) + " ALERT</strong></div>"
    body = body + "<div style='padding:12px;'>"
    body = body + "<h3>" + title + "</h3>"
    body = body + "<pre>" + details + "</pre></div></div>"
    
    client.send(ALERT_EMAIL, ADMIN_EMAIL, "[" + upper(level) + "] " + title, body)
}

sendAlert("Database Connection Failed", "Host: db.prod\nError: Connection refused\nTime: 14:35:00 UTC", "critical")
sendAlert("High Memory Usage", "Memory at 87%. Threshold: 80%", "warning")
sendAlert("Daily Backup Complete", "3 databases backed up successfully.", "info")
```

---

*Documentation generated from `E:\ezlib\ezemail\main.ez` — EZ Email Library*
