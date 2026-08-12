import ctypes
lib = ctypes.cdll.LoadLibrary("libcrypto.dll")
NID_X25519 = 950

lib.EVP_PKEY_CTX_new_id.restype = ctypes.c_void_p
lib.EVP_PKEY_keygen_init.argtypes = [ctypes.c_void_p]
lib.EVP_PKEY_keygen.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p)]
lib.EVP_PKEY_get_raw_private_key.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p]
lib.EVP_PKEY_get_raw_public_key.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p]

ctx = lib.EVP_PKEY_CTX_new_id(NID_X25519, None)
if ctx:
    print("CTX created")
    lib.EVP_PKEY_keygen_init(ctx)
    pkey_ptr = ctypes.c_void_p()
    res = lib.EVP_PKEY_keygen(ctx, ctypes.byref(pkey_ptr))
    if res == 1:
        print("Key generated")
        priv = ctypes.create_string_buffer(32)
        priv_len = ctypes.c_size_t(32)
        lib.EVP_PKEY_get_raw_private_key(pkey_ptr, priv, ctypes.byref(priv_len))
        print("Priv:", list(priv.raw))
        pub = ctypes.create_string_buffer(32)
        pub_len = ctypes.c_size_t(32)
        lib.EVP_PKEY_get_raw_public_key(pkey_ptr, pub, ctypes.byref(pub_len))
        print("Pub:", list(pub.raw))
    else:
        print("Keygen failed")
else:
    print("CTX failed")

