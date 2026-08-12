import asyncio
import websockets
import ssl
import base64
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

def encode_int(tag, val):
    wire_type = 0
    header = (tag << 3) | wire_type
    return encode_varint(header) + encode_varint(val)

def test_python_clienthello():
    appVer = encode_int(1, 2) + encode_int(2, 3000) + encode_int(3, 1035194821)
    
    ua = (encode_int(1, 14) +
          encode_bytes(2, appVer) +
          encode_bytes(3, b"000") +
          encode_bytes(4, b"000") +
          encode_bytes(5, b"0.1") +
          encode_bytes(7, b"Desktop") +
          encode_bytes(8, b"0.1") +
          encode_int(10, 0) +
          encode_bytes(11, b"en") +
          encode_bytes(12, b"US"))
          
    wi = encode_int(4, 0)
    
    dp = (encode_bytes(1, base64.b64decode("AAAAxA==")) +
          encode_bytes(2, base64.b64decode("BQ==")) +
          encode_bytes(3, base64.b64decode("78GVGg6LA3DS0JSnVYh9Isf23dS7FOeMnH9sdI7xyQU=")) +
          encode_bytes(4, base64.b64decode("AAAB")) +
          encode_bytes(5, base64.b64decode("whvbEp/eC+Yj5n+1Higb0v2C2s4cFwqp1F4Dv7Uew0E=")) +
          encode_bytes(6, base64.b64decode("u8YkuELI6WZOEnH9FYXsJAVPIulQcrVWncIR3Haa/j8Kyo/HsDSXSisaQtYqPMl1VmRYqG1OENNhEg0+89Mdig==")) +
          encode_bytes(7, base64.b64decode("oslHHv+RCp4a5lmJCiiHLQ==")) +
          encode_bytes(8, base64.b64decode("CgZNYWMgT1MSBggKEA8YBxgBIAEqFxiAUCABMAA4AUABSAFQAVgBYAFwAXgA")))
          
    cp = (encode_int(3, 0) +
          encode_bytes(5, ua) +
          encode_bytes(6, wi) +
          encode_int(12, 1) +
          encode_int(13, 1) +
          encode_bytes(19, dp) +
          encode_int(33, 0))
          
    priv = x25519.X25519PrivateKey.generate()
    ephemeral = priv.public_key().public_bytes(encoding=serialization.Encoding.Raw, format=serialization.PublicFormat.Raw)
    
    e_bytes = encode_bytes(1, ephemeral)
    p_bytes = encode_bytes(3, cp)
    ch_bytes = e_bytes + p_bytes
    hm_bytes = encode_bytes(2, ch_bytes)
    
    l = len(hm_bytes)
    # 3 byte frame
    frame_header = bytes([(l>>16)&255, (l>>8)&255, l&255])
    return frame_header + hm_bytes

async def test():
    ssl_context = ssl.create_default_context()
    
    async with websockets.connect("wss://web.whatsapp.com/ws/chat", ssl=ssl_context, origin="https://web.whatsapp.com", subprotocols=["chat"]) as ws:
        frame = test_python_clienthello()
        await ws.send(bytes([87, 65, 6, 2]) + frame)
        
        res = await ws.recv()
        print("Received length:", len(res))
        print("First bytes:", list(res[:10]))

asyncio.run(test())

