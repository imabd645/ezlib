import asyncio
import websockets
import ssl
from cryptography.hazmat.primitives.asymmetric import x25519
from cryptography.hazmat.primitives import serialization

def encode_varint(n):
    res = []
    while n > 127:
        res.append((n & 127) | 128)
        n >>= 7
    res.append(n & 127)
    return bytes(res)

def encode_bytes(tag, data):
    wire_type = 2
    header = (tag << 3) | wire_type
    return encode_varint(header) + encode_varint(len(data)) + data

def test_python_clienthello():
    priv = x25519.X25519PrivateKey.generate()
    ephemeral = priv.public_key().public_bytes(encoding=serialization.Encoding.Raw, format=serialization.PublicFormat.Raw)
    
    e_bytes = encode_bytes(1, ephemeral)
    hm_bytes = encode_bytes(2, e_bytes)
    
    l = len(hm_bytes)
    frame_header = bytes([(l>>16)&255, (l>>8)&255, l&255])
    return frame_header + hm_bytes

async def test():
    ssl_context = ssl.create_default_context()
    
    async with websockets.connect("wss://web.whatsapp.com/ws/chat", ssl=ssl_context, origin="https://web.whatsapp.com", subprotocols=["chat"]) as ws:
        frame = test_python_clienthello()
        await ws.send(bytes([87, 65, 6, 3]) + frame)
        
        try:
            res = await ws.recv()
            print("Received length:", len(res))
            print("First bytes:", list(res[:10]))
        except websockets.exceptions.ConnectionClosed as e:
            print("Connection closed:", e.code, e.reason)

asyncio.run(test())

