import ctypes
lib = ctypes.cdll.LoadLibrary("libcrypto.dll")
NID_X25519 = 950

lib.EVP_PKEY_CTX_new_id.restype = ctypes.c_void_p
lib.EVP_PKEY_CTX_new.restype = ctypes.c_void_p
lib.EVP_PKEY_new_raw_public_key.restype = ctypes.c_void_p
lib.EVP_PKEY_derive_init.argtypes = [ctypes.c_void_p]
lib.EVP_PKEY_derive_set_peer.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
lib.EVP_PKEY_derive.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p]

# Generate Key
ctx = lib.EVP_PKEY_CTX_new_id(NID_X25519, None)
lib.EVP_PKEY_keygen_init(ctx)
my_pkey = ctypes.c_void_p()
lib.EVP_PKEY_keygen(ctx, ctypes.byref(my_pkey))

# Server public key
pub = ctypes.create_string_buffer(32)
pub_len = ctypes.c_size_t(32)
lib.EVP_PKEY_get_raw_public_key(my_pkey, pub, ctypes.byref(pub_len))

peer_pkey = lib.EVP_PKEY_new_raw_public_key(NID_X25519, None, pub, 32)

derive_ctx = lib.EVP_PKEY_CTX_new(my_pkey, None)
lib.EVP_PKEY_derive_init(derive_ctx)
lib.EVP_PKEY_derive_set_peer(derive_ctx, peer_pkey)

secret = ctypes.create_string_buffer(32)
secret_len = ctypes.c_size_t(32)
lib.EVP_PKEY_derive(derive_ctx, secret, ctypes.byref(secret_len))

print("Secret:", list(secret.raw))

