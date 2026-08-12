import ctypes
ncrypt = ctypes.windll.ncrypt
prov = ctypes.c_void_p()
ncrypt.NCryptOpenStorageProvider(ctypes.byref(prov), ctypes.c_wchar_p("Microsoft Software Key Storage Provider"), 0)

key = ctypes.c_void_p()
res = ncrypt.NCryptCreatePersistedKey(prov, ctypes.byref(key), ctypes.c_wchar_p("X25519"), None, 0, 0)
print("X25519 CreatePersistedKey res:", hex(res & 0xFFFFFFFF))

if res == 0:
    ncrypt.NCryptFinalizeKey(key, 0)
    cbResult = ctypes.c_ulong(0)
    ncrypt.NCryptExportKey(key, None, ctypes.c_wchar_p("ECCPUBLICBLOB"), None, None, 0, ctypes.byref(cbResult), 0)
    print("X25519 Export size:", cbResult.value)
    if cbResult.value > 0:
        buf = ctypes.create_string_buffer(cbResult.value)
        ncrypt.NCryptExportKey(key, None, ctypes.c_wchar_p("ECCPUBLICBLOB"), None, buf, cbResult.value, ctypes.byref(cbResult), 0)
        print("X25519 Public Magic:", list(buf.raw[0:4]))

