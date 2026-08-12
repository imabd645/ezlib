# EZ FFI (`ffi` v2)

`ffi` is the official standard library for EZ that provides a robust, object-oriented abstraction over the native C Foreign Function Interface (FFI). It is designed to achieve full parity with Python's `ctypes`, providing a seamless and highly expressive bridge between EZ scripts and native C binaries.

With `ffi`, you can interact natively with shared libraries (`.dll`, `.so`, `.dylib`), raw memory pointers, structures, and C callbacks directly from EZ script without writing any C/C++ wrapper code.

---

## Installation
The `ffi` library comes bundled with the EZ compiler (v6+). You can import it globally using:
```ez
use "ffi"
```
Or with an alias to avoid polluting the global namespace:
```ez
use "ffi" as ffi
```

---

## 1. Library & Function Resolution

The entry point for FFI is loading a native dynamic library.

### `ffi.load(dll_name)`
Loads a native dynamic library (e.g., `.dll` on Windows, `.so` on Linux, `.dylib` on macOS) and returns a `Library` handle.

**Parameters:**
- `dll_name` *(String)*: The name or absolute path of the dynamic library (e.g., `"User32.dll"`).

**Returns:** A `Library` instance. Throws an `FFIError` if the library cannot be found or opened.

### `Library.func(name, restype = nil, argtypes = nil)`
Extracts a native function from the loaded library by its exported symbol name. It returns a `Function` object which caches its configuration.

**Parameters:**
- `name` *(String)*: The exported symbol name (e.g., `"MessageBoxA"`).
- `restype` *(Type Descriptor)*: *(Optional)* The return type of the function. Defaults to `ffi.c_int`.
- `argtypes` *(Array of Type Descriptors)*: *(Optional)* The expected argument types.

**Returns:** A `Function` object.

### `Function` API
Once a `Function` is resolved, you can set its signature and invoke it.
- **`fn.argtypes`**: An array of type descriptors defining the C function's parameters.
- **`fn.restype`**: A type descriptor defining the C function's return type.
- **`fn.sig(argtypes, restype)`**: A chainable shorthand to set both `argtypes` and `restype` at once.

**Invocation:**
`Function` objects implement the `__call__` method, meaning they can be executed directly as if they were standard EZ tasks.
```ez
lib = ffi.load("User32.dll")
msgBox = lib.func("MessageBoxA")

// Configure the signature
msgBox.argtypes = [ffi.c_void_p, ffi.c_char_p, ffi.c_char_p, ffi.c_uint32]
msgBox.restype = ffi.c_int

// Direct invocation
result = msgBox(0, "Hello, FFI!", "Title", 0)
```

Alternatively, you can use the explicit array-based invocation:
```ez
result = msgBox.call([0, "Hello, FFI!", "Title", 0])
```

---

## 2. Type Descriptors

Native libraries require strict ABI compliance, meaning EZ dynamically coerces its high-level types into precise C byte representations. `ffi` provides dictionaries that define these types.

| Type Descriptor | C Equivalent | Size (Bytes) | EZ Auto-Coercion |
| :--- | :--- | :--- | :--- |
| `ffi.c_int8` | `int8_t` | 1 | Number, Boolean (0/1) |
| `ffi.c_uint8` / `c_byte` | `uint8_t` | 1 | Number |
| `ffi.c_int16` | `int16_t` | 2 | Number |
| `ffi.c_uint16` | `uint16_t` | 2 | Number |
| `ffi.c_int32` / `c_int` | `int32_t` | 4 | Number, Boolean (0/1) |
| `ffi.c_uint32` / `c_uint` | `uint32_t` | 4 | Number |
| `ffi.c_int64` / `c_long` | `int64_t` | 8 | Number |
| `ffi.c_uint64` / `c_ulong` | `uint64_t` | 8 | Number |
| `ffi.c_float` | `float` | 4 | Number |
| `ffi.c_double` | `double` | 8 | Number |
| `ffi.c_bool` | `bool` | 1 | Boolean |
| `ffi.c_char_p` | `char*` | 8 | String (auto null-terminated) |
| `ffi.c_void_p` / `c_void` | `void*` | 8 | Integer (Address), Pointer |

*Note: When passing an EZ `String` to a `c_char_p` argument, `ffi` automatically allocates a temporary UTF-8 null-terminated C string in memory, passes its pointer to C, and cleans it up after the function returns.*

---

## 3. C Structures (`ffi.Structure`)

To interface with C functions that accept or return `struct` pointers, `ffi` allows you to define matching memory layouts in EZ. `Structure` respects standard C alignment and padding rules automatically.

### `ffi.Structure(name, fields)`
**Parameters:**
- `name` *(String)*: The name of the struct (for debugging).
- `fields` *(Array of Arrays)*: A list of `["fieldName", TypeDescriptor]` pairs.

**Returns:** A `Structure` factory object.

```ez
RECT = ffi.Structure("RECT", [
    ["left",   ffi.c_int32],
    ["top",    ffi.c_int32],
    ["right",  ffi.c_int32],
    ["bottom", ffi.c_int32]
])
```

### Structure API
- **`StructFactory.new()`**: Creates a new, zero-initialized EZ dictionary representing the struct fields.
- **`StructFactory.pack(inst)`**: Takes the EZ dictionary and serializes it into a raw C-memory `Pointer` buffer, respecting C alignment padding.
- **`StructFactory.unpack(buffer_or_address)`**: Reads a C-memory `Pointer` (or raw integer address) and deserializes it back into a named EZ dictionary.
- **`StructFactory.alloc()`**: Allocates an empty `Pointer` buffer perfectly sized for this structure.

---

## 4. Advanced Memory Management (`Pointer`)

When interfacing with complex C APIs, you often need to manually manage heap allocations and manipulate memory bytes. The `Pointer` model provides an RAII (Resource Acquisition Is Initialization) wrapper over raw memory addresses.

### Creating Pointers
- **`ffi.alloc(size_in_bytes)`**: Allocates zeroed memory on the heap and returns an owning `Pointer`. The memory is automatically freed when the `Pointer` is garbage collected by EZ.
- **`ffi.cast(raw_address)`**: Takes an integer representing a memory address and returns a "borrowed" `Pointer`. Borrowed pointers do not own the memory and will not free it upon garbage collection.

### Reading and Writing
`Pointer` objects expose strongly-typed methods to read/write memory at specific byte offsets relative to the pointer's base address.

- `.readInt8(offset)`, `.writeInt8(offset, val)`
- `.readUInt8(offset)`, `.writeUInt8(offset, val)`
- `.readInt16(offset)`, `.writeInt16(offset, val)`
- `.readUInt16(offset)`, `.writeUInt16(offset, val)`
- `.readInt32(offset)`, `.writeInt32(offset, val)`
- `.readUInt32(offset)`, `.writeUInt32(offset, val)`
- `.readInt64(offset)`, `.writeInt64(offset, val)`
- `.readFloat32(offset)`, `.writeFloat32(offset, val)`
- `.readFloat64(offset)`, `.writeFloat64(offset, val)`
- `.readString(offset = 0)`: Reads a null-terminated string starting at the offset.
- `.writeString(offset, str)`: Writes a null-terminated string to the offset.

### Manual Memory Control
- **`ptr.free()`**: Immediately frees the underlying C memory block. It sets an internal flag to prevent "Double Free" errors. Any subsequent read/write attempt will raise a safe "Use-After-Free" `MemoryError` instead of crashing the process.
- **`ptr.addr()`**: Returns the raw integer memory address.

---

## 5. String Utilities

- **`ffi.c_string(ez_string)`**: Allocates a new heap `Pointer` containing the UTF-8 null-terminated version of the provided EZ string. You own this pointer.
- **`ffi.string_at(address)`**: Reads a null-terminated string directly from a raw integer address.

---

## 6. Native Callbacks (`ffi.createCallback`)

Many C APIs (like `EnumWindows` or `qsort`) require you to pass a "function pointer". `ffi` allows you to wrap an EZ `task` inside a native C trampoline that the C library can execute.

### `ffi.createCallback(ez_task, restype, argtypes)`
Generates a native C function pointer that delegates execution back into the EZ interpreter.

**Parameters:**
- `ez_task` *(Task/Closure)*: The EZ function to execute.
- `restype` *(Type Descriptor)*: The type the C function expects as a return value.
- `argtypes` *(Array of Type Descriptors)*: The types of arguments the C function will pass.

**Returns:** A `Pointer` object representing the raw C function pointer.

```ez
use "ffi"

// 1. Define the EZ logic
task windowEnumCallback(hwnd, lParam) {
    out "Found window handle: " + str(hwnd)
    give 1 // Return 1 to continue enumeration
}

// 2. Wrap it in a C callback
cbPtr = ffi.createCallback(windowEnumCallback, ffi.c_int, [ffi.c_void_p, ffi.c_void_p])

// 3. Pass the callback pointer to the native library
user32 = ffi.load("User32.dll")
enumWindows = user32.func("EnumWindows", ffi.c_int, [ffi.c_void_p, ffi.c_void_p])

enumWindows(cbPtr.addr(), 0)

// 4. Free the callback trampoline when no longer needed
cbPtr.free()
```
*Warning: You must ensure the `cbPtr` remains alive (not garbage collected) for as long as the C library might call it.*

---

## Error Handling
`ffi` throws typed exceptions for safety boundary violations:
- **`FFIError`**: Thrown for missing DLLs, missing symbols, or un-coercible type arguments.
- **`MemoryError`**: Thrown for use-after-free, double-free, or attempting to free a borrowed pointer.
