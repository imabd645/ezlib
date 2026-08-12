import ctypes
import os
lib = ctypes.cdll.LoadLibrary("libcrypto.dll")
# int X25519(uint8_t out_shared_key[32], const uint8_t private_key[32], const uint8_t peer_public_value[32]);

X25519 = lib.X25519

priv = os.urandom(32)
basepoint = bytes([9] + [0]*31)
pub = ctypes.create_string_buffer(32)

res = X25519(pub, priv, basepoint)
print("Pubgen res:", res)

priv2 = os.urandom(32)
pub2 = ctypes.create_string_buffer(32)
X25519(pub2, priv2, basepoint)

secret = ctypes.create_string_buffer(32)
res = X25519(secret, priv, pub2)
print("Secret res:", res)

secret2 = ctypes.create_string_buffer(32)
X25519(secret2, priv2, pub)
print("Secret matched:", list(secret.raw) == list(secret2.raw))

