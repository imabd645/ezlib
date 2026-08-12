import ctypes
ncrypt = ctypes.windll.ncrypt
prov = ctypes.c_void_p()
ncrypt.NCryptOpenStorageProvider(ctypes.byref(prov), ctypes.c_wchar_p("Microsoft Software Key Storage Provider"), 0)

key = ctypes.c_void_p()
ncrypt.NCryptCreatePersistedKey(prov, ctypes.byref(key), ctypes.c_wchar_p("ECDH"), None, 0, 0)
prop = "curve25519\0".encode("utf-16le")
ncrypt.NCryptSetProperty(key, ctypes.c_wchar_p("ECCCurveName"), prop, len(prop), 0)

# Set Length to 255
length = ctypes.c_uint32(255)
ncrypt.NCryptSetProperty(key, ctypes.c_wchar_p("Length"), ctypes.byref(length), 4, 0)

res = ncrypt.NCryptFinalizeKey(key, 0)
print("FinalizeKey res:", hex(res & 0xFFFFFFFF))

cbResult = ctypes.c_ulong(0)
ncrypt.NCryptExportKey(key, None, ctypes.c_wchar_p("ECCPUBLICBLOB"), None, None, 0, ctypes.byref(cbResult), 0)
print("Export cbResult:", cbResult.value)
if cbResult.value > 0:
    buf = ctypes.create_string_buffer(cbResult.value)
    ncrypt.NCryptExportKey(key, None, ctypes.c_wchar_p("ECCPUBLICBLOB"), None, buf, cbResult.value, ctypes.byref(cbResult), 0)
    print("Public Magic:", list(buf.raw[0:4]))

