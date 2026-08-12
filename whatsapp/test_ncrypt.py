import ctypes
ncrypt = ctypes.windll.ncrypt
prov = ctypes.c_void_p()
ncrypt.NCryptOpenStorageProvider(ctypes.byref(prov), "Microsoft Software Key Storage Provider", 0)

key = ctypes.c_void_p()
ncrypt.NCryptCreatePersistedKey(prov, ctypes.byref(key), "ECDH", None, 0, 0)
prop = "curve25519\0".encode("utf-16le")
ncrypt.NCryptSetProperty(key, "ECCCurveName", prop, len(prop), 0)
ncrypt.NCryptFinalizeKey(key, 0)

cbResult = ctypes.c_ulong(0)
ncrypt.NCryptExportKey(key, None, "ECCPUBLICBLOB", None, None, 0, ctypes.byref(cbResult), 0)
buf = ctypes.create_string_buffer(cbResult.value)
ncrypt.NCryptExportKey(key, None, "ECCPUBLICBLOB", None, buf, cbResult.value, ctypes.byref(cbResult), 0)
print(list(buf.raw))

