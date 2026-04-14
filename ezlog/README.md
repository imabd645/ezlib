# EZ Programming Language

<div align="center">

![EZ Language](https://img.shields.io/badge/EZ-Language-blue?style=for-the-badge)
![Version](https://img.shields.io/badge/version-1.0-green?style=for-the-badge)
![C++17](https://img.shields.io/badge/C++-17-orange?style=for-the-badge)
![License](https://img.shields.io/badge/license-MIT-yellow?style=for-the-badge)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey?style=for-the-badge)

**A simple, readable programming language with natural English syntax**

[Quick Start](#quick-start) • [Documentation](#language-syntax) • [Built-ins](#built-in-functions) • [Examples](#examples) • [Internals](#language-internals)

</div>

---

## Table of Contents

- [About EZ](#about-ez)
- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Language Syntax](#language-syntax)
  - [Variables](#variables)
  - [Data Types](#data-types)
  - [String Interpolation](#string-interpolation)
  - [Operators](#operators)
  - [Control Flow](#control-flow)
  - [Functions (Tasks)](#functions-tasks)
  - [Default Parameters](#default-parameters)
  - [Arrays](#arrays)
  - [Dictionaries](#dictionaries)
  - [Object-Oriented Programming](#object-oriented-programming)
  - [Interfaces](#interfaces)
  - [Static Variables](#static-variables)
  - [Operator Overloading](#operator-overloading)
  - [Error Handling](#error-handling)
  - [Modules and Imports](#modules-and-imports)
  - [Async Programming](#async-programming)
- [Built-in Functions](#built-in-functions)
- [Package Manager](#package-manager)
- [Advanced Examples](#advanced-examples)
- [Language Internals](#language-internals)
- [FAQ](#faq)
- [Contributing](#contributing)
- [Roadmap](#roadmap)
- [License](#license)

---

## About EZ

**EZ** is a dynamically-typed, interpreted programming language designed from the ground up for readability and simplicity. It uses natural, English-like keywords and an intuitive syntax that reads almost like plain English — making it ideal for beginners while remaining expressive enough for advanced work.

EZ is implemented in modern **C++17** and features a hand-written lexer, recursive-descent parser, tree-walking interpreter, and a mark-and-sweep garbage collector with cycle detection.

### Why EZ?

- **Natural Syntax** — `out`, `when`, `repeat`, `task`, `give` instead of `print`, `if`, `for`, `function`, `return`
- **Zero Boilerplate** — no type declarations, no semicolons, no mandatory `main()`
- **Full OOP** — models (classes), inheritance, interfaces, visibility modifiers, `super`, `self`
- **Automatic Memory Management** — mark-and-sweep GC with cycle detection
- **Rich Standard Library** — string, math, file I/O, JSON, HTTP, SQLite, regex, crypto, and more
- **Async / Await** — `spawn` and `await` for concurrent programming
- **String Interpolation** — backtick template strings with embedded expressions
- **Interactive REPL** — experiment with code instantly
- **Built-in Package Manager** — install and share reusable modules from GitHub

---

## Features

### Core Language
- Dynamic typing with first-class `nil`, `bool`, `number`, `string`, `array`, `dictionary`
- First-class functions, closures, and lambda expressions (`(x) => x * 2`)
- Default parameter values in task definitions
- Template strings with embedded expressions using backtick syntax
- `and` / `or` / `not` logical operators (no `&&`, `||`, `!` required)
- `yes` / `no` as aliases for `true` / `false`
- Compound assignment operators: `+=`, `-=`, `*=`, `/=`
- Multi-line block comments `/* ... */` and single-line `//` and `#` comments
- Nested block comment support

### Control Flow
- `when` / `other` conditional branching
- `while` loop with `escape` (break) and `skip` (continue)
- `repeat i = 0 to 10` numeric for-loop
- `get x in collection` foreach loop over arrays and strings
- `try` / `catch` / `throw` exception handling

### OOP
- `model` keyword defines classes with `init` constructors
- `shown` (public) and `hidden` (private) visibility modifiers
- `extends` for single inheritance
- `super` to access parent class methods
- `self` for the current instance
- `implements` for interface enforcement
- `interface` keyword for defining contracts
- `static` members on models
- `struct` for lightweight plain-data objects
- Operator overloading via specially named methods (`__add__`, `__sub__`, etc.)

### Developer Experience
- Detailed stack traces on runtime errors including file and line number
- Call depth guard (max 500 frames) to catch infinite recursion
- Informative error messages throughout the lexer, parser, and interpreter
- Interactive REPL with multiline input support (brace-counting)

---

## Installation

### Prerequisites

| Dependency | Purpose |
|---|---|
| C++17 compiler (GCC 8+, Clang 7+, MSVC 2019+) | Building the interpreter |
| SQLite3 | Database functions |
| libcurl | HTTP client functions |
| pthreads / Windows threads | Async `spawn` |
| Winsock2 | Windows networking |

### Building from Source

**Linux / macOS:**
```bash
git clone https://github.com/imabd645/ez-language.git
cd ez-language

g++ -std=c++17 -O2 -o ez \
    main.cpp Lexer.cpp Parser.cpp Interpreter.cpp \
    Builtins.cpp GCObject.cpp GC.cpp \
    -lsqlite3 -lcurl -lpthread
```

**Windows (MinGW / MSYS2):**
```bash
g++ -std=c++17 -O2 -o ez.exe \
    main.cpp Lexer.cpp Parser.cpp Interpreter.cpp \
    Builtins.cpp GCObject.cpp GC.cpp \
    -lsqlite3 -lcurl -lws2_32 -lpthread -lwinmm
```

**Windows (MSVC):**
```bat
cl /std:c++17 /O2 /EHsc ^
   main.cpp Lexer.cpp Parser.cpp Interpreter.cpp ^
   Builtins.cpp GCObject.cpp GC.cpp ^
   sqlite3.lib libcurl.lib Ws2_32.lib
```

### Adding EZ to Your PATH (Windows)

1. Move `ez.exe` to a permanent folder, e.g. `C:\ez\`
2. Open **System Properties → Environment Variables → System Variables → Path**
3. Click **New** and add `C:\ez`
4. Restart your terminal — now `ez` works from anywhere

---

## Quick Start

### Hello World
```ez
out "Hello, World!"
```

### Running a Script
```bash
ez hello.ez
```

### Interactive REPL
```bash
ez
>>> out "Hello!"
Hello!
>>> x = 5 + 3
>>> out x
8
>>> exit
```

---

## Language Syntax

### Variables

Variables are dynamically typed and need no declaration keyword.

```ez
// Basic assignment
name = "Alice"
age = 30
active = true
nothing = nil

// Compound assignment
score += 10
health -= 5
speed *= 2
damage /= 3

// Multiple chained assignment
a = b = c = 0
```

Variables are automatically created in the current scope on first assignment. Assigning to an existing variable in an outer scope updates that outer variable (lexical scoping).

---

### Data Types

EZ has six primitive types and several reference types:

| Type | Description | Literal Example |
|---|---|---|
| `nil` | Absence of a value | `nil` |
| `bool` | Boolean | `true`, `false`, `yes`, `no` |
| `number` | 64-bit float | `42`, `3.14`, `-7` |
| `string` | Immutable text | `"hello"`, `'world'` |
| `array` | Ordered mutable list | `[1, 2, 3]` |
| `dictionary` | Key-value map | `{"a": 1, "b": 2}` |
| `function` | Task / lambda | `task foo() { }` |
| `model instance` | OOP object | `new Person("Alice", 30)` |

**Truthiness rules:** Everything is truthy except `nil` and `false`. The number `0`, the empty string `""`, and empty arrays `[]` are all truthy.

---

### String Interpolation

EZ supports backtick template strings for embedding expressions directly in strings:

```ez
name = "Alice"
age = 30
out `Hello, {name}! You are {age} years old.`
// → Hello, Alice! You are 30 years old.

// Any expression works inside { }
out `Next year you'll be {age + 1}.`
out `Pi is approximately {round(3.14159 * 100) / 100}.`

// Nested calls are fine
numbers = [1, 2, 3]
out `Array has {len(numbers)} items.`
```

Backtick strings desugar at the lexer level into a series of string concatenation operations, so they carry no runtime overhead beyond the concatenation itself.

---

### Operators

#### Arithmetic
```ez
5 + 3    // 8
10 - 4   // 6
3 * 7    // 21
20 / 4   // 5.0
17 % 5   // 2
```

#### Comparison
```ez
5 == 5   // true
5 != 3   // true
3 < 5    // true
5 <= 5   // true
10 > 5   // true
7 >= 7   // true
```

#### Logical
```ez
true and false   // false
true or false    // true
not true         // false
```

`and` and `or` are **short-circuit** operators: `and` stops on the first falsy value, `or` stops on the first truthy value.

#### String Concatenation
```ez
"Hello" + " " + "World"   // "Hello World"
"Count: " + str(42)       // "Count: 42"
```

---

### Control Flow

#### `when` / `other` (If / Else)

```ez
// Simple condition
when score >= 60 {
    out "Pass"
}

// With else
when x > 0 {
    out "Positive"
} other {
    out "Non-positive"
}

// Chained else-if
when grade >= 90 {
    out "A"
} other when grade >= 80 {
    out "B"
} other when grade >= 70 {
    out "C"
} other {
    out "F"
}

// One-liner
when x > 10 { out "big" }
```

#### `while` Loop

```ez
i = 0
while i < 5 {
    out i
    i += 1
}

// Infinite loop with escape
while true {
    input = in()
    when input == "quit" { escape }
    out "You said: " + input
}

// skip (continue)
i = 0
while i < 10 {
    i += 1
    when i % 2 == 0 { skip }
    out i   // only odd numbers
}
```

#### `repeat` Loop (Numeric For)

```ez
// Count up
repeat i = 1 to 5 {
    out i   // 1 2 3 4 5 (inclusive on both ends)
}

// Count down with manual decrement
repeat i = 10 to 1 {
    out i
    i -= 1
}

// Custom step
repeat i = 0 to 20 {
    out i
    i += 5   // 0 5 10 15 20
}
```

The loop variable `i` is created in the loop's own scope and is also accessible from within the body.

#### `get` Loop (Foreach)

```ez
// Over an array
fruits = ["apple", "banana", "orange"]
get fruit in fruits {
    out fruit
}

// Over a string (character by character)
get ch in "Hello" {
    out ch   // H e l l o
}

// With an index counter
items = ["x", "y", "z"]
idx = 0
get item in items {
    out `{idx}: {item}`
    idx += 1
}
```

---

### Functions (Tasks)

```ez
// No parameters, no return
task sayHello() {
    out "Hello!"
}

// With parameters
task greet(name) {
    out "Hi, " + name + "!"
}

// With return value (give)
task add(a, b) {
    give a + b
}

// Early return
task isEven(n) {
    when n % 2 == 0 { give true }
    give false
}

// Bare give (return nil)
task maybeStop(x) {
    when x < 0 { give }
    out x
}

// Recursive function
task factorial(n) {
    when n <= 1 { give 1 }
    give n * factorial(n - 1)
}

// Functions are first-class values
myFunc = add
out myFunc(3, 4)   // 7
```

#### Lambda Expressions

```ez
// Expression body
double = (x) => x * 2
add = (a, b) => a + b

// Block body
clamp = (x, lo, hi) => {
    when x < lo { give lo }
    when x > hi { give hi }
    give x
}

// Immediately invoked
result = ((x) => x * x)(5)   // 25

// Stored and passed around
ops = [
    (x) => x + 1,
    (x) => x * 2,
    (x) => x - 3
]
get op in ops {
    out op(10)   // 11, 20, 7
}
```

#### Closures

Functions capture their surrounding environment by reference, forming closures:

```ez
task makeAdder(n) {
    give (x) => x + n   // closes over n
}

add5 = makeAdder(5)
add10 = makeAdder(10)

out add5(3)    // 8
out add10(3)   // 13

// Counter closure
task makeCounter() {
    count = 0
    give () => {
        count += 1
        give count
    }
}

c = makeCounter()
out c()   // 1
out c()   // 2
out c()   // 3
```

---

### Default Parameters

Task parameters can have default values:

```ez
task greet(name, greeting = "Hello") {
    out greeting + ", " + name + "!"
}

greet("Alice")           // Hello, Alice!
greet("Bob", "Hi")       // Hi, Bob!

task createUser(name, age = 0, active = true) {
    give {"name": name, "age": age, "active": active}
}

u = createUser("Alice")
out u["active"]   // true
```

Default values are evaluated at call time, not at definition time.

---

### Arrays

```ez
// Create
nums = [1, 2, 3, 4, 5]
mixed = ["hello", 42, true, nil]
matrix = [[1, 0], [0, 1]]
empty = []

// Access / mutate
out nums[0]        // 1
nums[0] = 99
out nums[0]        // 99

// Nested access
out matrix[1][0]   // 0

// Built-in operations
push(nums, 6)            // append
last = pop(nums)         // remove & return last
size = len(nums)         // length
found = contains(nums, 3)  // boolean membership test
idx = indexOf(nums, 3)   // -1 if not found
sliced = slice(nums, 1, 3)  // [nums[1], nums[2]]
sorted = sort(nums)      // new sorted array
rev = reverse(nums)      // new reversed array

// Functional helpers
doubled = map(nums, (x) => x * 2)
evens = filter(nums, (x) => x % 2 == 0)
total = reduce(nums, (acc, x) => acc + x, 0)

// Range generation
r = range(1, 5)   // [1, 2, 3, 4, 5]
```

---

### Dictionaries

```ez
// Create
person = {
    "name": "Alice",
    "age": 30,
    "city": "Lahore"
}

// Access
out person["name"]   // Alice

// Add / update
person["email"] = "alice@example.com"
person["age"] = 31

// Check membership
when has_key(person, "email") {
    out "Has email"
}

// Iterate keys
get k in keys(person) {
    out k + " = " + str(person[k])
}

// Get all values
vals = values(person)

// Length
out len(person)
```

Dictionary keys are always strings. Values can be any EZ type, including other dictionaries or arrays.

---

### Object-Oriented Programming

#### Defining a Model

```ez
model Animal {
    init(name, sound) {
        self.name = name
        self.sound = sound
    }

    shown speak() {
        out self.name + " says " + self.sound
    }

    shown getName() {
        give self.name
    }

    hidden formatInternal() {
        give "[" + self.name + "]"
    }
}

a = new Animal("Dog", "Woof")
a.speak()              // Dog says Woof
out a.getName()        // Dog
// a.formatInternal()  // ERROR — hidden method
```

- `shown` methods are **public** (accessible from outside)
- `hidden` methods are **private** (accessible only within the model)
- `self` refers to the current instance

#### Inheritance

```ez
model Vehicle {
    init(make, speed) {
        self.make = make
        self.speed = speed
    }

    shown describe() {
        out `${self.make} going ${self.speed} km/h`
    }

    shown accelerate(amount) {
        self.speed += amount
    }
}

model Car extends Vehicle {
    init(make, speed, doors) {
        self.make = make
        self.speed = speed
        self.doors = doors
    }

    shown describe() {
        out `${self.make} car (${self.doors} doors) going ${self.speed} km/h`
    }

    shown honk() {
        out "Beep beep!"
    }
}

model SportsCar extends Car {
    init(make, doors) {
        self.make = make
        self.speed = 0
        self.doors = doors
        self.turbo = true
    }

    shown boost() {
        self.speed += 50
        out "TURBO BOOST!"
    }
}

sc = new SportsCar("Ferrari", 2)
sc.accelerate(100)
sc.boost()
sc.describe()   // Ferrari car (2 doors) going 150 km/h
sc.honk()
```

#### `super`

Call a parent class method from a child:

```ez
model Shape {
    init(color) {
        self.color = color
    }

    shown draw() {
        out "Drawing a " + self.color + " shape"
    }
}

model Circle extends Shape {
    init(color, radius) {
        self.color = color
        self.radius = radius
    }

    shown draw() {
        super.draw()
        out "  (circle with radius " + str(self.radius) + ")"
    }
}

c = new Circle("red", 5)
c.draw()
// Drawing a red shape
//   (circle with radius 5)
```

#### Static Members

Static members belong to the model itself, not to instances:

```ez
model Counter {
    static count = 0

    init() {
        Counter.count += 1
        self.id = Counter.count
    }

    shown getId() {
        give self.id
    }

    shown getTotal() {
        give Counter.count
    }
}

a = new Counter()
b = new Counter()
c = new Counter()

out a.getId()      // 1
out c.getId()      // 3
out a.getTotal()   // 3
```

#### Structs

Structs are lightweight models that auto-generate fields — useful for simple data containers:

```ez
struct Point {
    x, y
}

struct Color {
    r, g, b
}

p = new Point()
p.x = 10
p.y = 20
out `(${p.x}, ${p.y})`

c = new Color()
c.r = 255
c.g = 128
c.b = 0
```

---

### Interfaces

Interfaces define a contract — a set of methods that any implementing model must provide. EZ checks at runtime (when an instance is created) that all interface methods are present.

```ez
interface Printable {
    print
    getDescription
}

interface Serializable {
    serialize
    deserialize
}

model Document implements Printable, Serializable {
    init(title, content) {
        self.title = title
        self.content = content
    }

    shown print() {
        out "=== " + self.title + " ==="
        out self.content
    }

    shown getDescription() {
        give "Document: " + self.title
    }

    shown serialize() {
        give self.title + "|" + self.content
    }

    shown deserialize(data) {
        parts = split(data, "|")
        self.title = parts[0]
        self.content = parts[1]
    }
}

doc = new Document("Notes", "Hello world")
doc.print()
out doc.getDescription()
```

If a model claims to `implements` an interface but is missing a required method, EZ throws a runtime error when you try to instantiate it.

---

### Static Variables

The `static` keyword inside a task creates a variable that persists across calls — like a mini-closure state without needing a closure:

```ez
task nextId() {
    static id = 0
    id += 1
    give id
}

out nextId()   // 1
out nextId()   // 2
out nextId()   // 3
```

---

### Operator Overloading

You can define how your model responds to built-in operators by implementing specially named methods:

| Operator | Method Name |
|---|---|
| `+` | `__add__` |
| `-` | `__sub__` |
| `*` | `__mul__` |
| `/` | `__div__` |
| `==` | `__eq__` |
| `<` | `__lt__` |
| `>` | `__gt__` |
| `-` (unary) | `__neg__` |
| `not` (unary) | `__not__` |

```ez
model Vector {
    init(x, y) {
        self.x = x
        self.y = y
    }

    shown __add__(other) {
        give new Vector(self.x + other.x, self.y + other.y)
    }

    shown __sub__(other) {
        give new Vector(self.x - other.x, self.y - other.y)
    }

    shown __mul__(scalar) {
        give new Vector(self.x * scalar, self.y * scalar)
    }

    shown __eq__(other) {
        give self.x == other.x and self.y == other.y
    }

    shown __neg__() {
        give new Vector(-self.x, -self.y)
    }

    shown toString() {
        give `(${self.x}, ${self.y})`
    }
}

a = new Vector(1, 2)
b = new Vector(3, 4)

c = a + b          // calls __add__
out c.toString()   // (4, 6)

d = a * 3
out d.toString()   // (3, 6)

out (a == a)       // true
out (a == b)       // false
```

---

### Error Handling

```ez
// Basic try / catch
try {
    result = 10 / 0
} catch err {
    out "Caught: " + str(err)
}

// Throw a string
task safeDivide(a, b) {
    when b == 0 {
        throw "Division by zero!"
    }
    give a / b
}

try {
    out safeDivide(10, 2)    // 5
    out safeDivide(10, 0)    // throws
} catch e {
    out "Error: " + str(e)
}

// Throw a dictionary (structured error)
task validateAge(age) {
    when age < 0 {
        throw {"code": 400, "message": "Age cannot be negative"}
    }
    when age > 150 {
        throw {"code": 400, "message": "Age is unrealistically large"}
    }
    give age
}

try {
    validateAge(-5)
} catch e {
    out "Code: " + str(e["code"])
    out "Message: " + e["message"]
}

// Nested try / catch
try {
    try {
        throw "inner"
    } catch e {
        out "Inner caught: " + e
        throw "re-thrown from inner"
    }
} catch e {
    out "Outer caught: " + e
}
```

---

### Modules and Imports

```ez
// math_utils.ez
task square(x) { give x * x }
task cube(x) { give x * x * x }
PI = 3.14159265

// main.ez
use "math_utils.ez"

out square(4)    // 16
out cube(3)      // 27
out PI           // 3.14159265
```

`use` executes the target file in the current scope — all definitions in the imported file become available. Paths can be relative to the script's location.

---

### Async Programming

EZ supports concurrent programming through `spawn` and `await`:

```ez
task heavyWork(n) {
    sleep(n * 100)   // simulate work
    give n * n
}

// Run tasks concurrently
f1 = spawn(heavyWork, 3)
f2 = spawn(heavyWork, 4)
f3 = spawn(heavyWork, 5)

// Do other work here while tasks run in the background
out "Tasks running..."

// Wait for results
out await(f1)   // 9
out await(f2)   // 16
out await(f3)   // 25
```

#### Async HTTP

```ez
future = fetch("https://api.example.com/data", {
    "method": "POST",
    "body": json_stringify({"key": "value"}),
    "headers": {"Content-Type": "application/json"}
})

// ... do other work ...
response = await(future)
data = json_parse(response)
out data
```

---

## Built-in Functions

### I/O

| Function | Signature | Description |
|---|---|---|
| `out` | `out value` | Print a value to stdout with newline (keyword, not a function) |
| `in()` | `in()` | Read a line from stdin |
| `print(...)` | `print(a, b, c)` | Print multiple values separated by spaces |

### Type Conversion & Inspection

| Function | Description | Example |
|---|---|---|
| `str(x)` | Convert any value to string | `str(42)` → `"42"` |
| `num(x)` | Parse string or bool to number | `num("3.14")` → `3.14` |
| `type(x)` | Return type name as string | `type([])` → `"array"` |

### String Functions

| Function | Description |
|---|---|
| `len(s)` | Length in characters |
| `substr(s, start, length)` | Extract substring |
| `split(s, delim)` | Split into array |
| `join(arr, delim)` | Join array into string |
| `replace(s, old, new)` | Replace all occurrences |
| `trim(s)` | Remove leading/trailing whitespace |
| `upper(s)` | Uppercase |
| `lower(s)` | Lowercase |
| `indexOf(s, sub)` | Index of substring (-1 if missing) |
| `charAt(s, i)` | Character at index |
| `startsWith(s, prefix)` | Boolean prefix check |
| `endsWith(s, suffix)` | Boolean suffix check |
| `reverse(s)` | Reverse string |
| `repeat_str(s, n)` | Repeat string n times |
| `pad_left(s, width, char)` | Left-pad to width |
| `pad_right(s, width, char)` | Right-pad to width |

### Array Functions

| Function | Description |
|---|---|
| `len(arr)` | Number of elements |
| `push(arr, val)` | Append element (mutates) |
| `pop(arr)` | Remove and return last element |
| `slice(arr, start, end)` | Extract subarray |
| `reverse(arr)` | Return reversed copy |
| `sort(arr)` | Return sorted copy |
| `contains(arr, val)` | Boolean membership test |
| `indexOf(arr, val)` | Index of element (-1 if missing) |
| `range(start, end)` | Generate `[start, ..., end]` inclusive |
| `map(arr, fn)` | Apply fn to each element |
| `filter(arr, fn)` | Keep elements where fn returns true |
| `reduce(arr, fn, init)` | Fold array to a single value |
| `flatten(arr)` | Flatten one level of nesting |
| `unique(arr)` | Remove duplicate values |
| `zip(a, b)` | Pair elements from two arrays |

### Math Functions

| Function | Description |
|---|---|
| `abs(x)` | Absolute value |
| `floor(x)` | Round toward negative infinity |
| `ceil(x)` | Round toward positive infinity |
| `round(x)` | Round to nearest integer |
| `sqrt(x)` | Square root |
| `pow(x, y)` | x to the power y |
| `min(a, b)` | Smaller of two numbers |
| `max(a, b)` | Larger of two numbers |
| `rand()` | Random float in [0, 1) |
| `randint(lo, hi)` | Random integer in [lo, hi] |
| `sin(x)` | Sine (radians) |
| `cos(x)` | Cosine (radians) |
| `tan(x)` | Tangent (radians) |
| `log(x)` | Natural logarithm |
| `log10(x)` | Base-10 logarithm |
| `exp(x)` | e^x |

### File System

| Function | Description |
|---|---|
| `read_file(path)` | Read entire file as string |
| `write_file(path, data)` | Write string to file (overwrites) |
| `append_file(path, data)` | Append string to file |
| `file_exists(path)` | Boolean existence check |
| `delete_file(path)` | Delete a file |
| `list_dir(path)` | Array of filenames in directory |
| `create_dir(path)` | Create directory |
| `file_size(path)` | File size in bytes |
| `copy_file(src, dst)` | Copy a file |
| `move_file(src, dst)` | Move / rename a file |

### JSON

| Function | Description |
|---|---|
| `json_parse(str)` | Parse JSON string → EZ value |
| `json_stringify(val)` | Serialize EZ value → JSON string |

### HTTP

| Function | Description |
|---|---|
| `http_get(url)` | Synchronous GET, returns body string |
| `http_post(url, body)` | Synchronous POST |
| `fetch(url, options)` | Async HTTP request, returns future |

`fetch` `options` dictionary keys: `"method"`, `"body"`, `"headers"` (dictionary).

### SQLite Database

| Function | Description |
|---|---|
| `db_open(path)` | Open (or create) a SQLite database |
| `db_execute(db, sql)` | Execute SQL (no results returned) |
| `db_query(db, sql)` | Execute SQL, return array of dictionaries |
| `db_close(db)` | Close a database connection |
| `db_last_insert_id(db)` | ID of the last INSERT |
| `db_begin(db)` | Begin a transaction |
| `db_commit(db)` | Commit the current transaction |
| `db_rollback(db)` | Roll back the current transaction |

### Regular Expressions

| Function | Description |
|---|---|
| `regex_match(str, pattern)` | True if the full string matches |
| `regex_search(str, pattern)` | Return first matching substring |
| `regex_replace(str, pattern, repl)` | Replace all matches |
| `regex_find_all(str, pattern)` | Array of all matches |

### Dictionary Utilities

| Function | Description |
|---|---|
| `keys(dict)` | Array of all keys |
| `values(dict)` | Array of all values |
| `has_key(dict, key)` | Boolean key membership check |

### Async / Concurrency

| Function | Description |
|---|---|
| `spawn(fn, args...)` | Run fn asynchronously, return future |
| `await(future)` | Block until future completes, return result |
| `sync(future)` | Alias for `await` |
| `sleep(ms)` | Sleep for ms milliseconds |



---

## Package Manager

EZ ships with a built-in package manager that installs packages from GitHub.

### CLI Commands

```bash
ez install <package>            # Install latest version
ez install <package> <branch>   # Install specific branch/version
ez list                         # List installed packages
ez init <name>                  # Scaffold a new package
```

Packages are installed into a local `ez_packages/` folder. The convention is that a GitHub repo named `EZmy-package` is installed with `ez install my-package`.

### Creating a Package

1. Create `package.ez` (package manifest as JSON):
```json
{
  "name": "my-package",
  "version": "1.0.0",
  "description": "Does something useful",
  "author": "Your Name",
  "main": "main.ez"
}
```

2. Write your `main.ez` with exported tasks and values

3. Push the repo to GitHub as `EZ<package-name>`

### Using an Installed Package

```ez
use "my-package"
result = myPackageFunction()
```

---

## Advanced Examples

### Fibonacci (Recursive and Iterative)

```ez
// Recursive
task fib(n) {
    when n <= 1 { give n }
    give fib(n - 1) + fib(n - 2)
}

// Iterative (faster)
task fibIter(n) {
    when n <= 1 { give n }
    a = 0
    b = 1
    repeat i = 2 to n {
        c = a + b
        a = b
        b = c
    }
    give b
}

out fib(10)       // 55
out fibIter(40)   // 102334155
```

### Higher-Order Functions

```ez
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

// Pipeline: double all evens, sum them
result = reduce(
    map(
        filter(numbers, (x) => x % 2 == 0),
        (x) => x * 2
    ),
    (acc, x) => acc + x,
    0
)
out result   // 60 (2+4+6+8+10)*2

// Sort by custom comparator (using sort + a key-extraction trick)
words = ["banana", "apple", "cherry", "date"]
// Sort by length
sorted = sort(map(words, (w) => [len(w), w]))
get pair in sorted {
    out pair[1]
}
```

### Mini Database App

```ez
db = db_open("users.db")

db_execute(db, "
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        email TEXT UNIQUE,
        created_at TEXT
    )
")

task addUser(name, email) {
    db_execute(db, `INSERT INTO users (name, email, created_at)
        VALUES ('${name}', '${email}', datetime('now'))`)
    give db_last_insert_id(db)
}

task getUsers() {
    give db_query(db, "SELECT * FROM users ORDER BY name")
}

task deleteUser(id) {
    db_execute(db, `DELETE FROM users WHERE id = ${id}`)
}

db_begin(db)
try {
    addUser("Alice", "alice@example.com")
    addUser("Bob", "bob@example.com")
    db_commit(db)
} catch e {
    db_rollback(db)
    out "Failed: " + str(e)
}

users = getUsers()
get u in users {
    out `[${u["id"]}] ${u["name"]} <${u["email"]}>`
}

db_close(db)
```

### Concurrent Web Requests

```ez
urls = [
    "https://jsonplaceholder.typicode.com/posts/1",
    "https://jsonplaceholder.typicode.com/posts/2",
    "https://jsonplaceholder.typicode.com/posts/3"
]

// Launch all requests in parallel
futures = []
get url in urls {
    push(futures, spawn(http_get, url))
}

// Collect results
get f in futures {
    data = json_parse(await(f))
    out data["title"]
}
```

### Class Hierarchy with Interfaces

```ez
interface Drawable {
    draw
    area
}

model Shape implements Drawable {
    init(color) {
        self.color = color
    }

    shown draw() {
        out "Shape [" + self.color + "]"
    }

    shown area() {
        give 0
    }
}

model Circle extends Shape {
    init(color, r) {
        self.color = color
        self.radius = r
    }

    shown draw() {
        out `Circle r=${self.radius} [${self.color}]`
    }

    shown area() {
        give 3.14159 * self.radius * self.radius
    }
}

model Rectangle extends Shape {
    init(color, w, h) {
        self.color = color
        self.width = w
        self.height = h
    }

    shown draw() {
        out `Rectangle ${self.width}x${self.height} [${self.color}]`
    }

    shown area() {
        give self.width * self.height
    }
}

shapes = [
    new Circle("red", 5),
    new Rectangle("blue", 4, 6),
    new Circle("green", 3)
]

get s in shapes {
    s.draw()
    out `  Area = ${s.area()}`
}
```

---

## Language Internals

This section describes how EZ is implemented, which is useful if you want to contribute or understand the codebase.

### Architecture Overview

```
Source Code (.ez)
     │
     ▼
  Lexer (Lexer.cpp / Lexer.h)
     │  Produces a flat list of Tokens
     ▼
  Parser (Parser.cpp / Parser.h)
     │  Recursive-descent; produces an AST (tree of StmtPtr / ExprPtr)
     ▼
  Interpreter (Interpreter.cpp / Interpreter.h)
     │  Tree-walker; evaluates the AST directly
     │  Uses Environment chain for variable scoping
     │  Calls built-in functions from Builtins.cpp / GUIBuiltins.cpp
     ▼
  GC (GC.cpp / GC.h / GCObject.cpp / GCObject.h)
     │  Mark-and-sweep collector triggered every N allocations
     ▼
  Output / Side Effects
```

### Lexer (`Lexer.cpp`)

The lexer converts raw source text into a flat `std::vector<Token>`. Key points:

- Single-pass, character-by-character scanning
- Keyword table (`std::unordered_map<string, TokenType>`) for reserved words
- Nested block comment support (`/* /* */ */`)
- `#` and `//` line comment support
- Backtick string interpolation is **desugared at the lexer level** — `\`Hello {name}\`` is immediately expanded into a chain of `STRING + IDENTIFIER + STRING` tokens wrapped in parentheses
- Numbers are always parsed as `double`
- `yes` / `no` are recognized as aliases for `true` / `false`

### Parser (`Parser.cpp`)

The parser uses **recursive descent** with explicit operator precedence levels:

```
expression → assignment
assignment → logicalOr  ( "=" | "+=" | "-=" | "*=" | "/=" )
logicalOr  → logicalAnd ( "or" logicalAnd )*
logicalAnd → equality   ( "and" equality )*
equality   → comparison ( ( "==" | "!=" ) comparison )*
comparison → term       ( ( "<" | "<=" | ">" | ">=" ) term )*
term       → factor     ( ( "+" | "-" ) factor )*
factor     → unary      ( ( "*" | "/" | "%" ) unary )*
unary      → ( "not" | "-" ) unary | call
call       → primary    ( "(" args ")" | "[" index "]" | "." name )*
primary    → literal | identifier | "(" expr ")" | lambda | "new" | "self"
```

The parser emits a tree of `shared_ptr<Stmt>` and `shared_ptr<Expr>` nodes. Statement types include `WhenStmt`, `WhileStmt`, `RepeatStmt`, `GetStmt`, `TaskStmt`, `ModelStmt`, `TryStmt`, etc. Expression types include `BinaryExpr`, `CallExpr`, `LambdaExpr`, `PropertyAccessExpr`, `DictionaryExpr`, etc.

### Interpreter (`Interpreter.cpp`)

The interpreter is a **tree-walking evaluator**. It visits each AST node and returns a `Value`. Key mechanisms:

- **Environments** (`Environment`) form a linked list (child → parent) for lexical scoping
- **`ReturnException`, `BreakException`, `ContinueException`** are C++ exceptions used to implement `give`, `escape`, and `skip` — they unwind the call stack naturally
- **Call stack** is tracked explicitly in `callStack: vector<CallFrame>` for error reporting
- **Call depth guard** (`MAX_CALL_DEPTH = 500`) prevents stack overflow
- **`toString` override** — if an instance has a `toString` method, `str()` and `out` will call it

### Value System (`Value.h`)

All EZ values are instances of the `Value` struct, which holds a `std::variant` over the possible types:

```
nullptr_t   → nil
bool        → boolean
double      → number
shared_ptr<string>       → string
shared_ptr<EZArray>      → array
shared_ptr<EZFunction>   → user-defined function
shared_ptr<NativeFunction> → built-in function
shared_ptr<EZClass>      → model (class)
shared_ptr<EZInstance>   → object instance
shared_ptr<EZDictionary> → dictionary
shared_ptr<future<Value>> → async future
shared_ptr<EZSuper>       → super proxy
```

Strings use `shared_ptr<std::string>` for cheap copy semantics while avoiding excess allocation.

### Garbage Collector (`GC.cpp`, `GCObject.cpp`)

EZ uses a **mark-and-sweep** garbage collector with **cycle detection**:

1. Every heap-allocated object (`EZArray`, `EZDictionary`, `EZFunction`, `EZClass`, `EZInstance`, `Environment`) extends `GCObject`
2. `GCObject`'s constructor registers the object in a global intrusive linked list maintained by `GarbageCollector`
3. Every N allocations (default threshold: 50,000), the GC runs:
   - **Phase 1 — Reset**: All marks cleared
   - **Phase 2 — Mark**: Starting from roots (global env, current env, the interpreter's saved environment stack, and explicit temporary roots), recursively mark all reachable objects
   - **Phase 3 — Sweep**: Call `gc_clear()` on all unmarked objects to sever internal `shared_ptr` references, breaking reference cycles so that the reference count drops to zero and `delete` is called naturally
4. Temporary roots (`addTemporaryRoot` / `removeTemporaryRoot`) protect objects that are live in C++ local variables but not yet reachable from any environment

### Error Reporting

Runtime errors include a **full stack trace**:

```
Runtime Error: Division by zero
  at [line 4] in lib/math.ez
  at calculate() in lib/math.ez:12
  at processData() in app.ez:8
  at main() in app.ez:20
```

The stack trace is built from `callStack: vector<CallFrame>`, where each frame stores the function name, source filename, and line number.

---

## FAQ

**Q: Do I need semicolons?**
No. Newlines serve as statement terminators. Blank lines are ignored. Inside `{ }` blocks, newlines are consumed freely.

**Q: Is EZ case-sensitive?**
Yes. `Name` and `name` are different identifiers. Keywords like `when`, `task`, `out` are lowercase only.

**Q: Can I use EZ for production software?**
EZ is currently a learning and scripting language. It is stable for small to medium automation scripts, but large-scale production use should wait for further optimization work.

**Q: What is the maximum recursion depth?**
500 call frames. Exceeding this throws a runtime error with a stack trace.

**Q: How does EZ handle integer vs floating-point?**
All numbers are stored as 64-bit IEEE 754 doubles internally. When printing, whole numbers display without a decimal point (`42` not `42.0`).

**Q: Can I import from a URL?**
Not directly yet. You can use `http_get` to download code to disk and then `use` the local path.

**Q: Does EZ support Unicode?**
Strings are stored as `std::string` (bytes). Basic ASCII Unicode escapes work in string literals. Full Unicode grapheme cluster support is planned.

**Q: How do I install packages without internet?**
Copy the package folder manually into `ez_packages/<package-name>/` in your project directory.

---

## Contributing

Contributions are very welcome! Here's how:

### Reporting Bugs
- Use the [GitHub issue tracker](https://github.com/imabd645/ez-language/issues)
- Include a minimal reproducible script (`.ez` file)
- State your OS, compiler version, and EZ build date

### Submitting Pull Requests
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes with clear, commented code
4. Test against existing `.ez` examples
5. Commit: `git commit -m "feat: add my feature"`
6. Push and open a PR with a clear description

### Good First Issues
- Add more built-in math / string functions in `Builtins.cpp`
- Improve error messages in `Parser.cpp`
- Write example programs in `examples/`
- Add documentation for the GUI framework

### Code Style
- 4-space indentation
- Follow existing naming: `camelCase` for variables, `PascalCase` for classes
- Add comments on non-obvious logic
- Keep functions focused and small

---





## License

EZ is released under the **MIT License**.

```
MIT License

Copyright (c) 2024 Abdullah Masood

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Acknowledgments

- Inspired by [Crafting Interpreters](https://craftinginterpreters.com/) (Nystrom), Python, and Lua
- Built with modern C++17
- Uses SQLite3, libcurl, and the Windows API
- Mark-and-sweep GC design informed by the CPython and V8 collectors

---

## Contact

- **GitHub**: [github.com/imabd645/ez-language](https://github.com/imabd645/ez-language)
- **Issues**: [github.com/imabd645/ez-language/issues](https://github.com/imabd645/ez-language/issues)

---

**Made with ❤️ by Abdullah Masood**

⭐ Star the repo if you find EZ useful!
