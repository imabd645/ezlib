# websocket — RFC 6455 client for EZ

> **Import:** `use "websocket"`
> **Install:** `ez install websocket`
> **Depends on:** `socket`, `crypto`

A WebSocket client in pure EZ. Handshake, masking, fragmentation, ping/pong
and close handling — no native library.

```ez
use "websocket"

ws = WebSocket("ws://localhost:8080/chat")
ws.connect()

ws.send("hello")
out ws.receive()

ws.close()
```

---

## Quick Start

```ez
use "websocket"

ws = WebSocket("ws://localhost:8080/feed")
ws.header("Authorization", "Bearer " + token)
ws.timeout(30000)
ws.connect()

ws.send(to_json({ "subscribe": "prices" }))

while ws.isOpen() {
    message = ws.receive()
    when message == nil { escape }        # peer closed
    handle(parse_json(message))
}
```

`receive()` returns `nil` once the peer closes, which is the loop's exit
condition.

## API

### Connecting

| Call | Effect |
| --- | --- |
| `WebSocket(url)` | Build a client. `ws://host:port/path` |
| `ws.connect()` | Perform the HTTP upgrade handshake |
| `ws.timeout(ms)` | Socket timeout, default 10000. Returns the client |
| `ws.header(name, value)` | Extra handshake header. Returns the client |
| `ws.isOpen()` | Whether the connection is live |

### Sending

| Call | Sends |
| --- | --- |
| `ws.send(text)` | A text frame |
| `ws.sendBytes(list)` | A binary frame from byte values |
| `ws.ping(payload)` | A ping; the peer should pong |

### Receiving

| Call | Result |
| --- | --- |
| `ws.receive()` | Next message as text, or nil when closed |
| `ws.receiveMessage()` | `{ opcode, text, payload }` — keeps binary bytes |

**Ping and close are handled for you.** A ping is answered with a pong and a
close is acknowledged before hanging up, both inside `receive()`. A connection
that ignores pings gets dropped by the peer, and making every caller remember
that is a trap — so it is not optional here.

Fragmented messages are reassembled: `receive()` returns whole messages, never
partial frames.

### Closing

```ez
ws.close()                       # normal, code 1000
ws.closeWith(1001, "going away")
ws.closeCode                     # code the peer sent, after it closes
ws.closeReason
```

## Protocol notes

**Client frames are masked, server frames are not.** The RFC requires it — the
rule exists to stop a malicious page steering an intermediary proxy into
caching a forged response. This client masks everything it sends with a fresh
random key per frame.

**Byte-exactness matters, and EZ strings are UTF-8.** Building a frame with
`chr()` does not work: `chr(255)` becomes the two-byte sequence `C3 BF` and
the frame is corrupt. Every frame here is assembled through a raw buffer and
converted with `buf_to_str`, which is byte-exact — the same route `socket`
uses to read.

The byte helpers are exported because they are useful on their own:

```ez
bytesToStr([0, 255, 128])    # byte-exact string
strToBytes(data)             # back to byte values
intToBytes(300, 2)           # [1, 44] — big-endian
xorByte(a, b)  bitAnd(a, b)  bitOr(a, b)
```

EZ has no bitwise operators, so those are computed bit by bit.

## Limits

- **No `wss://`.** TLS is not implemented; `connect()` refuses with an
  explanation rather than failing obscurely later. Put a TLS-terminating
  proxy in front, or connect over a private network.
- **Client only.** There is no server side — accepting a WebSocket needs
  integration with `web`'s request loop, which this does not have.
- **Blocking.** `receive()` waits for the socket. For a program that must do
  other work, run it on a `spawn`ed thread.
- **Not thread-safe.** One connection is one conversation.
- **No permessage-deflate.** Compression is not negotiated.
- Outgoing messages are never fragmented; incoming fragments are reassembled.

## Testing

```
ez test.ez
```

65 tests with no server required, since framing and the handshake are pure
functions over bytes. Includes the **worked example from RFC 6455 §1.3** —
the published key/accept pair — plus byte-exactness, all three length
encodings (7-bit, 16-bit, 64-bit), masking round-trips, short reads at every
boundary, and control-frame handling.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `WebSocket` client: handshake, send, receive, close |
| `frame.ez` | RFC 6455 frame encoding and incremental decoding |
| `bytes.ez` | Byte-exact string building, big-endian ints, XOR and bit ops |

## License

MIT
