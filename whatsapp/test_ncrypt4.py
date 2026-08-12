import ctypes
ncrypt = ctypes.windll.ncrypt
prov = ctypes.c_void_p()
ncrypt.NCryptOpenStorageProvider(ctypes.byref(prov), "Microsoft Software Key Storage Provider", 0)

key = ctypes.c_void_p()
ncrypt.NCryptCreatePersistedKey(prov, ctypes.byref(key), "ECDH", None, 0, 0)
prop = "curve25519\0".encode("utf-16le")
ncrypt.NCryptSetProperty(key, "ECCCurveName", prop, len(prop), 0)

NCRYPT_ALLOW_PLAINTEXT_EXPORT_FLAG = ctypes.c_uint32(3)
ncrypt.NCryptSetProperty(key, "Export Policy", ctypes.byref(NCRYPT_ALLOW_PLAINTEXT_EXPORT_FLAG), 4, 0)

ncrypt.NCryptFinalizeKey(key, 0)

cbResult = ctypes.c_ulong(0)
res = ncrypt.NCryptExportKey(key, None, "ECCPRIVATEBLOB", None, None, 0, ctypes.byref(cbResult), 0)
print("Export size res:", hex(res & 0xFFFFFFFF), "size:", cbResult.value)
if cbResult.value > 0:
    buf = ctypes.create_string_buffer(cbResult.value)
    res = ncrypt.NCryptExportKey(key, None, "ECCPRIVATEBLOB", None, buf, cbResult.value, ctypes.byref(cbResult), 0)
    b = list(buf.raw)
    magic = b[0:4]
    cbKey = b[4]
    print("Magic:", magic)
    print("cbKey:", cbKey)

