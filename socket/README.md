# socket — TCP and UDP sockets for EZ

Windows Sockets 2 (`ws2_32.dll`) over pure FFI. No C++.

```ez
use "socket"
```

Everything is a method on `Conn`, `TcpListener` or `UdpSocket`. Modules import
flat in EZ, so a library that exported `send`, `recv`, `bind`, `listen` and
`close` as bare functions would drop five of the most collision-prone names in
the language into every program that used it.

---

## Quick start

**Server**

```ez
use "socket"

srv = tcp_listen("127.0.0.1", 8080)
conn = srv.accept()

out conn.ip + ":" + str(conn.port) + " says " + conn.recv(1024)
conn.sendAll("hello back")
conn.close()
srv.close()
```

**Client**

```ez
use "socket"

c = tcp_connect("example.com", 80, 5000)      # hostname, 5s timeout
c.sendAll("GET / HTTP/1.0\r\nHost: example.com\r\n\r\n")
out c.recvAll()
c.close()
```

**UDP**

```ez
u = udp_open()
u.sendTo("127.0.0.1", 9999, "ping")

srv = udp_open()
srv.bindTo("127.0.0.1", 9999)
m = srv.recvFrom(1024)        # { "data": ..., "ip": ..., "port": ... }
```

---

## API

### Constructors

| | |
|---|---|
| `tcp_listen(ip, port, backlog = 128, reuseAddr = false)` → `TcpListener` | `ip` must be numeric (`"0.0.0.0"`, `"127.0.0.1"`) |
| `tcp_connect(host, port, timeoutMs = 0)` → `Conn` | `host` may be a hostname **or** a dotted quad |
| `udp_open()` → `UdpSocket` | |
| `resolve_host(host)` → `string` or `nil` | hostname → dotted quad |

### `Conn`

| | |
|---|---|
| `.ip` / `.port` | the peer's address |
| `recv(maxBytes = 4096)` | up to `maxBytes`; `""` when the peer closed |
| `recvAll(chunkSize = 4096)` | read until the peer closes |
| `send(data)` | one send; returns bytes accepted, which may be fewer than offered |
| `sendAll(data)` | loops until everything is written |
| `setTimeout(ms)` | recv/send deadline; `0` blocks forever |
| `shutdownSend()` | half-close — stop sending, keep reading |
| `close()` | |

### `TcpListener`

`accept()` → `Conn` · `close()` · `.ip` / `.port`

### `UdpSocket`

`sendTo(host, port, data)` · `recvFrom(maxBytes = 4096)` · `bindTo(ip, port)` ·
`setTimeout(ms)` · `setBroadcast(on)` · `close()`

### Errors

Every failure raises a `SocketError` carrying the Winsock code and a readable
explanation, rather than returning `-1` or `false`:

```
SocketError: connect to 127.0.0.1:1 failed (WSA 10061: connection refused --
nothing is listening on that address and port)
```

`wsa_message(code)` maps a code on its own.

---

## Things worth knowing

**Binary data is safe.** `recv` takes bytes by pointer and length, so a payload
containing NUL bytes arrives intact. An earlier version read the receive buffer
as a C string, so 10 bytes containing a NUL at index 3 arrived as 3 — no binary
protocol could work.

**Ports are claimed exclusively.** `SO_REUSEADDR` does **not** mean on Windows
what it means on Unix. On Unix it permits rebinding a port stuck in `TIME_WAIT`;
on Windows it *also* lets a second process bind a port that is already being
listened on, with the stack choosing which socket receives a given connection —
so any local process could quietly hijack a server's port. `tcp_listen`
therefore sets `SO_EXCLUSIVEADDRUSE` by default. Pass `reuseAddr = true` only if
you know you want the other behaviour.

**Set a timeout on anything facing a network.** Without one, `recv` on a peer
that goes quiet holds the thread indefinitely — which is how a slow client turns
into a hung server.

**Use `sendAll`, not `send`, unless you mean it.** TCP may accept fewer bytes
than offered; `send` reports that honestly and `sendAll` loops.

---

## Not covered

- IPv6 (`AF_INET` only)
- Non-blocking / `select` / IOCP — sockets are blocking, so concurrency means
  `spawn()` per connection
- TLS — terminate at a reverse proxy, or use `http` for HTTPS client requests
- Unix domain sockets (Windows only)

---

## Tests

```
ez test_socket.ez
```

27 checks over loopback: address round-tripping, DNS, error mapping, TCP echo,
peer-address reporting, binary payloads with embedded NULs, recv timeouts, UDP
datagrams, exclusive binding, and operations on closed sockets. Nothing requires
an internet connection.

`test_server.ez` / `test_client.ez` are a runnable two-terminal example.
