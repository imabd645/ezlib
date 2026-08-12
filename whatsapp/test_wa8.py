import base64

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

print("cp_hex = \"" + cp.hex() + "\"")

