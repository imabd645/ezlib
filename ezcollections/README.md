# ezcollections — Data Structures Library for EZ

> **Version:** 2.0  
> **Import:** `use "ezcollections"`  
> **File:** `E:\ezlib\ezcollections\main.ez`

---

## Overview

`ezcollections` provides a suite of classic computer science data structures implemented in pure EZ. It includes **Set**, **Map**, **Queue**, **Stack**, **List**, and **Counter** — all built on top of EZ's native dictionary and array primitives.

Use this library when you need typed, encapsulated data structures with clearly defined semantics rather than raw arrays/dictionaries.

---

## Quick Start

```ez
use "ezcollections"

s = Set()
s.add("apple").add("banana").add("apple")
out s.size()     # → 2

q = Queue()
q.enqueue("task1").enqueue("task2")
out q.dequeue()  # → "task1"
```

---

## Models

### `Set`

An unordered collection of **unique values**. Backed by a dictionary for O(1) lookups.

#### `Set.init()`
Creates an empty Set.

```ez
s = Set()
```

#### `Set.add(val)` → `self`
Adds a value. Duplicate values are silently ignored. Values are keyed by their string representation.

```ez
s = Set()
s.add(1).add(2).add(1)
out s.size()   # → 2
```

#### `Set.has(val)` → `boolean`
Returns `true` if the value exists in the set.

```ez
s = Set()
s.add("hello")
out s.has("hello")   # → true
out s.has("world")   # → false
```

#### `Set.remove(val)` → `self`
Removes a value. No-op if the value doesn't exist.

```ez
s = Set()
s.add("a").add("b")
s.remove("a")
out s.has("a")   # → false
```

#### `Set.size()` → `number`
Returns the number of unique elements.

```ez
s = Set()
s.add("x").add("y")
out s.size()  # → 2
```

#### `Set.clear()` → `self`
Removes all elements.

```ez
s = Set()
s.add("a")
s.clear()
out s.size()  # → 0
```

#### `Set.toArray()` → `array`
Converts the set to an array. **Order is not guaranteed** (dictionary iteration order).

```ez
s = Set()
s.add("c").add("a").add("b")
arr = s.toArray()
out arr  # → ["c", "a", "b"] (order may vary)
```

#### `Set.print()`
Prints a string representation to stdout.

```ez
s = Set()
s.add(1).add(2).add(3)
s.print()   # → Set{1, 2, 3}
```

---

### `Map`

An ordered key-value store that supports **any type as a key** (keys are stringified internally). Provides a richer API than raw dictionaries.

#### `Map.init()`
Creates an empty Map.

```ez
m = Map()
```

#### `Map.set(key, val)` → `self`
Stores a key-value pair. Overwrites if key already exists.

```ez
m = Map()
m.set("name", "Alice").set("age", 30)
```

#### `Map.get(key)` → `value | nil`
Retrieves the value for a key. Returns `nil` if key not found.

```ez
m = Map()
m.set("x", 42)
out m.get("x")   # → 42
out m.get("y")   # → nil
```

#### `Map.has(key)` → `boolean`
Checks if a key exists.

```ez
m = Map()
m.set("foo", "bar")
out m.has("foo")   # → true
out m.has("baz")   # → false
```

#### `Map.remove(key)` → `self`
Removes a key-value pair. No-op if key doesn't exist.

```ez
m = Map()
m.set("a", 1)
m.remove("a")
out m.has("a")  # → false
```

#### `Map.size()` → `number`
Returns the number of key-value pairs.

```ez
m = Map()
m.set(1, "one").set(2, "two")
out m.size()  # → 2
```

#### `Map.keys()` → `array`
Returns all keys as an array of strings (keys are always stored as strings).

```ez
m = Map()
m.set("alpha", 1).set("beta", 2)
out m.keys()  # → ["alpha", "beta"]
```

#### `Map.values()` → `array`
Returns all values as an array.

```ez
m = Map()
m.set("a", 10).set("b", 20)
out m.values()  # → [10, 20]
```

---

### `Queue`

A **FIFO** (First-In-First-Out) data structure. Elements are added at the back and removed from the front.

#### `Queue.init()`
Creates an empty Queue.

```ez
q = Queue()
```

#### `Queue.enqueue(val)` → `self`
Adds a value to the back of the queue.

```ez
q = Queue()
q.enqueue("first").enqueue("second").enqueue("third")
```

#### `Queue.dequeue()` → `value | nil`
Removes and returns the front element. Returns `nil` if empty.

```ez
q = Queue()
q.enqueue("a").enqueue("b")
out q.dequeue()  # → "a"
out q.dequeue()  # → "b"
out q.dequeue()  # → nil
```

#### `Queue.peek()` → `value | nil`
Returns the front element without removing it.

```ez
q = Queue()
q.enqueue("first")
out q.peek()   # → "first"
out q.size()   # → 1 (not removed)
```

#### `Queue.isEmpty()` → `boolean`
Returns `true` if the queue has no elements.

```ez
q = Queue()
out q.isEmpty()  # → true
q.enqueue("x")
out q.isEmpty()  # → false
```

#### `Queue.size()` → `number`
Returns the number of elements.

#### `Queue.print()`
Prints queue contents with `<-` arrows showing direction.

```ez
q = Queue()
q.enqueue("A").enqueue("B").enqueue("C")
q.print()   # → Queue[A <- B <- C]
```

---

### `Stack`

A **LIFO** (Last-In-First-Out) data structure. Elements are added and removed from the top.

#### `Stack.init()`
Creates an empty Stack.

```ez
st = Stack()
```

#### `Stack.push(val)` → `self`
Adds a value to the top of the stack.

```ez
st = Stack()
st.push(1).push(2).push(3)
```

#### `Stack.pop()` → `value | nil`
Removes and returns the top element. Returns `nil` if empty.

```ez
st = Stack()
st.push("a").push("b")
out st.pop()  # → "b"
out st.pop()  # → "a"
out st.pop()  # → nil
```

#### `Stack.peek()` → `value | nil`
Returns the top element without removing it.

```ez
st = Stack()
st.push(99)
out st.peek()   # → 99
out st.size()   # → 1
```

#### `Stack.isEmpty()` → `boolean`
Returns `true` if stack is empty.

#### `Stack.size()` → `number`
Returns the number of elements.

---

### `List`

A doubly-ordered dynamic list with index-based access, `prepend`, and `reverse` operations.

#### `List.init()`
Creates an empty List.

```ez
lst = List()
```

#### `List.append(val)` → `self`
Adds a value to the end.

```ez
lst = List()
lst.append("a").append("b").append("c")
```

#### `List.prepend(val)` → `self`
Adds a value to the beginning. O(n) operation.

```ez
lst = List()
lst.append("b").prepend("a")
out lst.get(0)  # → "a"
out lst.get(1)  # → "b"
```

#### `List.get(index)` → `value | nil`
Returns the element at a given 0-based index. Returns `nil` for out-of-bounds.

```ez
lst = List()
lst.append("x").append("y")
out lst.get(0)  # → "x"
out lst.get(5)  # → nil
```

#### `List.remove(index)` → `self`
Removes the element at the given index. No-op for out-of-bounds.

```ez
lst = List()
lst.append("a").append("b").append("c")
lst.remove(1)
out lst.get(1)  # → "c"
```

#### `List.reverse()` → `self`
Reverses the list in-place.

```ez
lst = List()
lst.append(1).append(2).append(3)
lst.reverse()
out lst.get(0)  # → 3
```

#### `List.size()` → `number`
Returns the number of elements.

#### `List.print()`
Prints list with `->` arrows.

```ez
lst = List()
lst.append("a").append("b").append("c")
lst.print()   # → List[a -> b -> c]
```

---

### `Counter`

Counts occurrences of items. Useful for frequency analysis, word counting, and tallying.

#### `Counter.init()`
Creates an empty Counter.

```ez
c = Counter()
```

#### `Counter.add(item)` → `self`
Increments the count for `item` by 1. Creates a new entry with count 1 if not seen before.

```ez
c = Counter()
c.add("apple").add("banana").add("apple").add("apple")
```

#### `Counter.get(item)` → `number`
Returns the count for an item. Returns `0` if the item was never added.

```ez
c = Counter()
c.add("dog").add("dog").add("cat")
out c.get("dog")   # → 2
out c.get("cat")   # → 1
out c.get("fish")  # → 0
```

#### `Counter.print()`
Prints all items and their counts.

```ez
c = Counter()
c.add("a").add("b").add("a")
c.print()  # → Counter{a: 2, b: 1}
```

---

## Edge Cases & Notes

### Key Types in Set and Map
Both `Set` and `Map` convert all keys/values to their string representation using `str()` before storing. This means:
- `set.add(1)` and `set.add("1")` are **the same element**.
- `map.set(42, "val")` and `map.set("42", "val")` are **the same key**.

```ez
s = Set()
s.add(1)
out s.has("1")  # → true  (because 1 → "1" internally)
```

### Queue Performance
`Queue.dequeue()` rebuilds the underlying array each time (O(n)). For high-throughput queues with thousands of operations per second, consider a linked-list approach or native array shifting.

### Stack vs Array
`Stack.pop()` calls EZ's built-in `pop()` which removes the last element of an array. This is O(1) and efficient.

### List.prepend Performance
`List.prepend()` is O(n) — it rebuilds the entire array. For prepend-heavy workloads, consider using a `Stack` (reversed) or implementing a linked list manually.

### Counter Key Collisions
Like `Map` and `Set`, `Counter` uses `str(item)` as the key. Numbers and their string equivalents collide.

---

## Full Example: Word Frequency Counter

```ez
use "ezcollections"

text = "the quick brown fox jumps over the lazy dog the fox"
words = split(text, " ")

counter = Counter()
get w in words {
    counter.add(w)
}

out "Word frequencies:"
out "  'the': " + str(counter.get("the"))   # → 3
out "  'fox': " + str(counter.get("fox"))   # → 2
out "  'dog': " + str(counter.get("dog"))   # → 1
counter.print()
```

---

## Full Example: BFS using Queue

```ez
use "ezcollections"

# Build a simple graph as adjacency dictionary
graph = {
    "A": ["B", "C"],
    "B": ["D"],
    "C": ["D", "E"],
    "D": [],
    "E": []
}

# Breadth-first search
task bfs(start) {
    visited = Set()
    queue = Queue()
    
    queue.enqueue(start)
    visited.add(start)
    order = []
    
    while not queue.isEmpty() {
        node = queue.dequeue()
        push(order, node)
        
        neighbors = graph[node]
        get n in neighbors {
            when not visited.has(n) {
                visited.add(n)
                queue.enqueue(n)
            }
        }
    }
    give order
}

out bfs("A")  # → ["A", "B", "C", "D", "E"]
```

---

## Full Example: Undo Stack

```ez
use "ezcollections"

history = Stack()
document = ""

task typeText(text) {
    history.push(document)
    document = document + text
    out "Document: " + document
}

task undo() {
    prev = history.pop()
    when prev == nil {
        out "Nothing to undo."
        give
    }
    document = prev
    out "Undone. Document: " + document
}

typeText("Hello ")
typeText("World")
typeText("!")
undo()
undo()
```

---

*Documentation generated from `E:\ezlib\ezcollections\main.ez` — EZ Collections Library v2.0*
