# serial

Talk to hardware over a COM port.

```ez
use "serial"

port = openPort("COM3", { "baud": 9600 })
port.writeLine("MEASURE")
out port.readLine()
port.close()
```

Arduinos, GPS modules, scales, barcode scanners, RS-485 sensors, modems, PLCs,
and every industrial device built before Ethernet was cheap speak serial. It is
the oldest interface still in daily use, and the one nothing else replaced.

## Install

```
ez install serial
```

Windows only — this is `kernel32`'s serial API directly, with no other
dependency.

## Finding the port

```ez
out listPorts()        # ["COM3", "COM7", "COM10"]
isPresent("COM3")      # true
```

Sorted numerically, not alphabetically: `COM10` sorts after `COM9`, and a list
reading COM1, COM10, COM3 looks broken.

## Opening

```ez
port = openPort("COM3", {
    "baud": 115200,
    "dataBits": 8,          # 5–8
    "parity": "none",       # none | odd | even | mark | space
    "stopBits": "1",        # "1" | "1.5" | "2"
    "flowControl": "none",  # none | rtscts | xonxoff | dsrdtr
    "readTimeout": 1000,    # ms
    "writeTimeout": 1000
})
```

Everything except `baud` has a sensible default — 9600 8N1, no flow control.
A setting name that is not one of these raises rather than being ignored,
because `baudRate: 115200` silently opening at 9600 is a long afternoon.

The same is true of the values. An unknown parity does not fall back to
`none`: a device expecting even parity and given none returns a stream of
plausible-looking wrong bytes, which is far harder to diagnose than a refusal.

Failures name themselves:

```
SerialError: COM9 does not exist -- listPorts() shows what does
SerialError: COM3 is already open in another program
```

## Reading

Every read has a deadline. This matters more than anything else in the package:
Windows' default is to block until a byte arrives, so a program reading from a
sensor that has been unplugged waits forever, with no error and no output.

```ez
port.readLine()             # up to \n, \r\n handled
port.readLine(5000)         # …with a 5s deadline
port.readUntil(">")         # up to a delimiter
port.readExactly(16)        # exactly 16 bytes, or a timeout
port.read(64)               # up to 64 bytes, as text
port.readBytes(64)          # …as a byte array
port.readAvailable()        # whatever has arrived, no waiting
port.waiting()              # how many bytes are queued
```

`readLine` and `readUntil` reassemble a reply that arrives in pieces, which it
usually does — a 40-byte NMEA sentence at 9600 baud lands in three or four
chunks. Anything past the delimiter is kept for the next call, so a device that
talks faster than the program reads does not lose every other message.

`readExactly` is for fixed-length frames. `readBytes` returning short is the
commonest source of a half-parsed reading.

## Writing

```ez
port.send("AT")                  # no terminator
port.writeLine("AT")             # …with \n
port.writeLine("AT", "\r\n")     # …with something else
port.writeBytes([2, 65, 66, 3])  # raw
port.flushOutput()               # block until the bytes have left
```

Most devices answer nothing without a terminator, and they disagree about which
one. `\r\n` is the common answer for AT-command modems, `\n` for Arduinos.

## Control lines

```ez
port.setDtr(true)
port.setRts(false)
port.pulseDtr()        # low, then high — resets an Arduino
port.signals()         # { cts, dsr, ring, carrier }
```

An Arduino resets when DTR drops and rises. That is how the bootloader is
entered, and it is also why a program that opens the port and reads immediately
gets the bootloader's output instead of the sketch's — give it a moment, or
call `discard()`.

## Housekeeping

```ez
port.discard()     # throw away buffered input and output
port.isOpen()
port.close()
```

`discard()` is worth calling after opening. A device that has been talking to
nobody has a queue of stale readings, and the first "measurement" a program
reads is often minutes old.

`close()` twice is not an error — cleanup code should not have to know.

## Example: an Arduino

```ez
use "serial"

port = openPort("COM3", { "baud": 115200, "readTimeout": 2000 })

# The board resets when the port opens; wait for the sketch, not the bootloader.
pause(2000)
port.discard()

repeat i = 1 to 10 {
    port.writeLine("READ")
    out port.readLine()
}
port.close()
```

## Example: a GPS module

```ez
port = openPort("COM7", { "baud": 9600, "readTimeout": 3000 })
while true {
    line = port.readLine()
    when startsWith(line, "$GPGGA") {
        fields = split(line, ",")
        out "lat " + fields[2] + fields[3] + "  lon " + fields[4] + fields[5]
    }
}
```

## Example: an AT-command modem

```ez
port = openPort("COM4", { "baud": 115200, "flowControl": "rtscts" })
port.writeLine("AT", "\r\n")
out port.readUntil("OK", 2000)
port.close()
```

## How it is tested

Most of a serial package cannot be tested without a device on the other end,
and the parts that can be are the parts that fail *silently*: a DCB with one
field at the wrong offset opens the port, returns success, and hands back
garbage at the wrong baud rate.

So the struct packing is checked against **Windows' own DCB**. `BuildCommDCB`
turns a mode string into the struct the kernel expects; this package's bytes
and Windows' bytes are compared field by field across ten configurations. Port
enumeration is checked against the ports .NET reports. If the machine has a COM
port, the suite opens it for real.

```
ez serial/test.ez        # 119 checks
```

## Not included

- **A modbus or NMEA parser.** Those are protocols on top of the wire; this is
  the wire. `text` and `struct`-style unpacking cover most framing.
- **Asynchronous or overlapped I/O.** Reads have deadlines instead, which
  covers the cases a script needs without an event loop.
- **Virtual port creation.** Use com0com or a USB adapter.

## See also

- `os` — process and environment access
- `text` — parsing what the device sends
- `log` — recording a session
