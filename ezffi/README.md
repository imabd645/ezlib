# EZ FFI (`ezffi`)

`ezffi` is the official standard library for EZ that provides a robust, object-oriented abstraction over the native C Foreign Function Interface (FFI). It is the EZ equivalent to Python's `ctypes`.

With `ezffi`, you no longer need to manually manage raw `os_load_lib`, `os_get_func`, or complex memory operations. It handles dynamic argument unwrapping, memory allocation, structure reading/writing, and float-register signatures automatically!

## Installation
```bash
ez install ezffi 1.0.0
```

## Loading and Calling DLLs
You can easily load any Windows `.dll` (or `.so` / `.dylib` on other systems) and invoke its exported functions directly.

```ez
use "ezffi"

# 1. Load the dynamic library
user32 = ezffi.load("User32.dll")

# 2. Extract a function by name and specify its return type (pass false for signature if no floats are used)
MessageBox = user32.func("MessageBoxA", ezffi.INT, false)

# 3. Call it with a dynamic array of arguments!
MessageBox.call([0, "Hello from ezffi!", "EZ ctypes", 0])
```

## Passing Floating Point Arguments
Native C libraries (like OpenGL, Physics engines, or PDF generators) often require floating point numbers (`float` or `double`). 

To prevent floating-point register corruption across the FFI boundary, you must provide a **Signature Array** when extracting the function. This tells `ezffi` which registers to use when packing the arguments.

```ez
mathlib = ezffi.load("math.dll")

# Extract function, declare return type, and declare argument types
calculate = mathlib.func("calculate_velocity", ezffi.FLOAT, [ezffi.FLOAT, ezffi.INT])

# Safely call with float and integer!
result = calculate.call([42.5, 10])
```

## Raw Memory Management (`Pointer`)
When interfacing with C libraries, you often need to allocate raw memory blocks and pass their memory addresses (pointers). The `Pointer` model gives you safe, high-level access to raw memory:

```ez
# Allocate 64 bytes of heap memory
ptr = ezffi.alloc(64)

# Write different data types at specific memory offsets
ptr.writeString(0, "Raw Memory Data")
ptr.writeFloat64(32, 3.14159)
ptr.writeByte(40, 255)

# Read the data back
print(ptr.readString())       # Outputs: Raw Memory Data
print(ptr.readFloat64(32))    # Outputs: 3.14159

# Prevent memory leaks!
ptr.free()
```

## Available Types
The `ezffi` package provides standard constants for types:
- `ezffi.INT`: Standard integer (`intptr_t`)
- `ezffi.PTR`: Memory Address (`void*`)
- `ezffi.FLOAT`: 32-bit floating point number
- `ezffi.DOUBLE`: 64-bit floating point number
- `ezffi.STR`: Null-terminated C string (`char*`)
