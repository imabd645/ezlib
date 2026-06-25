# EZ Socket

A full WinSock2 networking library allowing pure EZ code to act as TCP clients or servers.

## Installation
```javascript
use "socket"
```

## Creating a Server
```javascript
server = socket.create()
socket.bind(server, "0.0.0.0", 8080)
socket.listen(server, 10)

print("Listening on port 8080...")
client = socket.accept(server)

msg = socket.recv(client, 1024)
socket.send(client, "Echo: " + msg)

socket.close(client)
socket.close(server)
```

## Creating a Client
```javascript
client = socket.create()
socket.connect(client, "127.0.0.1", 8080)

socket.send(client, "Hello Server!")
msg = socket.recv(client, 1024)
print("Server replied: " + msg)

socket.close(client)
```
