import ctypes
ncrypt = ctypes.windll.ncrypt
prov = ctypes.c_void_p()
ncrypt.NCryptOpenStorageProvider(ctypes.byref(prov), "Microsoft Software Key Storage Provider", 0)

key = ctypes.c_void_p()
ncrypt.NCryptCreatePersistedKey(prov, ctypes.byref(key), "ECDH", None, 0, 0)
prop = "curve25519\0".encode("utf-16le")
ncrypt.NCryptSetProperty(key, "ECCCurveName", prop, len(prop), 0)

# Set Length to 255
length = ctypes.c_uint32(255)
ncrypt.NCryptSetProperty(key, "Length", ctypes.byref(length), 4, 0)

# Allow plaintext export
NCRYPT_ALLOW_PLAINTEXT_EXPORT_FLAG = ctypes.c_uint32(3)
ncrypt.NCryptSetProperty(key, "Export Policy", ctypes.byref(NCRYPT_ALLOW_PLAINTEXT_EXPORT_FLAG), 4, 0)

res = ncrypt.NCryptFinalizeKey(key, 0)
print("FinalizeKey res:", hex(res & 0xFFFFFFFF))

cbResult = ctypes.c_ulong(0)
res = ncrypt.NCryptExportKey(key, None, "ECCPUBLICBLOB", None, None, 0, ctypes.byref(cbResult), 0)
print("Export cbResult:", cbResult.value)
if cbResult.value > 0:
    buf = ctypes.create_string_buffer(cbResult.value)
    res = ncrypt.NCryptExportKey(key, None, "ECCPUBLICBLOB", None, buf, cbResult.value, ctypes.byref(cbResult), 0)
    print("Public Magic:", list(buf.raw[0:4]))

