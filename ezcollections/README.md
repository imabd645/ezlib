# EZ Collections Library v2.0

Professional data structures for the EZ programming language. Go beyond arrays with Sets, Maps, Queues, Stacks, Lists, and Counters - all implemented with clean object-oriented APIs.

![Version](https://img.shields.io/badge/version-2.0-blue)
![Data Structures](https://img.shields.io/badge/structures-6-green)
![OOP](https://img.shields.io/badge/OOP-yes-purple)

## 🚀 Quick Start

```bash
ez install collections
```

```ez
use "collections"

// Create a Set (unique values)
mySet = Set()
mySet.add(1).add(2).add(1)  // Only stores unique values
mySet.print()  // Set{1, 2}

// Create a Map (key-value pairs)
myMap = Map()
myMap.set("name", "Alice")
     .set("age", 25)
out myMap.get("name")  // "Alice"

// Create a Queue (FIFO)
queue = Queue()
queue.enqueue("first").enqueue("second")
out queue.dequeue()  // "first"
```

---

## 📚 Data Structures Overview

| Structure | Use Case | Operations | Time Complexity |
|-----------|----------|------------|-----------------|
| **Set** | Unique values, membership testing | add, has, remove | O(1) average |
| **Map** | Key-value pairs, dictionaries | set, get, has | O(1) average |
| **Queue** | FIFO, task processing, BFS | enqueue, dequeue | O(1) / O(n) |
| **Stack** | LIFO, undo systems, DFS | push, pop | O(1) |
| **List** | Ordered collection, prepend/append | append, prepend, reverse | O(1) / O(n) |
| **Counter** | Frequency counting, histograms | add, get | O(1) |

---

## 📖 Complete API Reference

### Set - Unique Value Collection

Stores only unique values. Perfect for removing duplicates and membership testing.

#### Constructor
```ez
mySet = Set()
```

#### Methods

##### `add(value)`
Adds a value to the set (duplicates ignored).

**Returns:** Self (for chaining)

**Example:**
```ez
set = Set()
set.add(1)
   .add(2)
   .add(1)  // Ignored - already exists
set.print()  // Set{1, 2}
```

---

##### `has(value)`
Checks if value exists in set.

**Returns:** Boolean (`yes` or `no`)

**Example:**
```ez
set = Set()
set.add(5)
out set.has(5)  // yes
out set.has(10) // no
```

---

##### `remove(value)`
Removes a value from the set.

**Returns:** Self (for chaining)

**Example:**
```ez
set = Set()
set.add(1).add(2).add(3)
set.remove(2)
set.print()  // Set{1, 3}
```

---

##### `size()`
Returns number of elements.

**Returns:** Number

**Example:**
```ez
set = Set()
set.add(1).add(2).add(3)
out set.size()  // 3
```

---

##### `clear()`
Removes all elements.

**Returns:** Self (for chaining)

**Example:**
```ez
set.clear()
out set.size()  // 0
```

---

##### `toArray()`
Converts set to array.

**Returns:** Array of values

**Example:**
```ez
set = Set()
set.add(3).add(1).add(2)
arr = set.toArray()
out arr  // [3, 1, 2] (order not guaranteed)
```

---

##### `print()`
Prints set contents.

**Example:**
```ez
set.print()  // Set{1, 2, 3}
```

---

### Map - Key-Value Store

Dictionary/HashMap for storing key-value pairs.

#### Constructor
```ez
myMap = Map()
```

#### Methods

##### `set(key, value)`
Stores a key-value pair.

**Returns:** Self (for chaining)

**Example:**
```ez
map = Map()
map.set("name", "Alice")
   .set("age", 25)
   .set("city", "NYC")
```

---

##### `get(key)`
Retrieves value for key.

**Returns:** Value or `nil` if not found

**Example:**
```ez
map = Map()
map.set("name", "Bob")
out map.get("name")     // "Bob"
out map.get("missing")  // nil
```

---

##### `has(key)`
Checks if key exists.

**Returns:** Boolean

**Example:**
```ez
map = Map()
map.set("age", 30)
out map.has("age")   // yes
out map.has("name")  // no
```

---

##### `remove(key)`
Removes a key-value pair.

**Returns:** Self (for chaining)

**Example:**
```ez
map.set("temp", "data")
map.remove("temp")
```

---

##### `size()`
Returns number of entries.

**Returns:** Number

**Example:**
```ez
map = Map()
map.set("a", 1).set("b", 2)
out map.size()  // 2
```

---

##### `keys()`
Returns array of all keys.

**Returns:** Array of keys

**Example:**
```ez
map = Map()
map.set("name", "Alice")
   .set("age", 25)
keys = map.keys()
out keys  // ["name", "age"]
```

---

##### `values()`
Returns array of all values.

**Returns:** Array of values

**Example:**
```ez
map = Map()
map.set("a", 1).set("b", 2)
vals = map.values()
out vals  // [1, 2]
```

---

### Queue - FIFO Collection

First-In-First-Out data structure. Perfect for task processing and BFS algorithms.

#### Constructor
```ez
queue = Queue()
```

#### Methods

##### `enqueue(value)`
Adds value to end of queue.

**Returns:** Self (for chaining)

**Example:**
```ez
queue = Queue()
queue.enqueue("first")
     .enqueue("second")
     .enqueue("third")
```

---

##### `dequeue()`
Removes and returns first value.

**Returns:** Value or `nil` if empty

**Example:**
```ez
queue = Queue()
queue.enqueue("A").enqueue("B")
out queue.dequeue()  // "A"
out queue.dequeue()  // "B"
out queue.dequeue()  // nil (empty)
```

---

##### `peek()`
Returns first value without removing.

**Returns:** Value or `nil` if empty

**Example:**
```ez
queue = Queue()
queue.enqueue("first")
out queue.peek()  // "first"
out queue.peek()  // "first" (still there)
```

---

##### `isEmpty()`
Checks if queue is empty.

**Returns:** Boolean

**Example:**
```ez
queue = Queue()
out queue.isEmpty()  // yes
queue.enqueue(1)
out queue.isEmpty()  // no
```

---

##### `size()`
Returns number of elements.

**Returns:** Number

**Example:**
```ez
queue = Queue()
queue.enqueue(1).enqueue(2).enqueue(3)
out queue.size()  // 3
```

---

##### `print()`
Prints queue contents (front to back).

**Example:**
```ez
queue.print()  // Queue[first <- second <- third]
```

---

### Stack - LIFO Collection

Last-In-First-Out data structure. Perfect for undo systems and DFS algorithms.

#### Constructor
```ez
stack = Stack()
```

#### Methods

##### `push(value)`
Adds value to top of stack.

**Returns:** Self (for chaining)

**Example:**
```ez
stack = Stack()
stack.push(1)
     .push(2)
     .push(3)
```

---

##### `pop()`
Removes and returns top value.

**Returns:** Value or `nil` if empty

**Example:**
```ez
stack = Stack()
stack.push("A").push("B")
out stack.pop()  // "B" (last in)
out stack.pop()  // "A"
out stack.pop()  // nil (empty)
```

---

##### `peek()`
Returns top value without removing.

**Returns:** Value or `nil` if empty

**Example:**
```ez
stack = Stack()
stack.push("top")
out stack.peek()  // "top"
out stack.peek()  // "top" (still there)
```

---

##### `isEmpty()`
Checks if stack is empty.

**Returns:** Boolean

**Example:**
```ez
stack = Stack()
out stack.isEmpty()  // yes
stack.push(1)
out stack.isEmpty()  // no
```

---

##### `size()`
Returns number of elements.

**Returns:** Number

**Example:**
```ez
stack = Stack()
stack.push(1).push(2).push(3)
out stack.size()  // 3
```

---

### List - Enhanced Array

Ordered collection with prepend, append, and reverse operations.

#### Constructor
```ez
list = List()
```

#### Methods

##### `append(value)`
Adds value to end.

**Returns:** Self (for chaining)

**Example:**
```ez
list = List()
list.append(1)
    .append(2)
    .append(3)
list.print()  // List[1 -> 2 -> 3]
```

---

##### `prepend(value)`
Adds value to beginning.

**Returns:** Self (for chaining)

**Example:**
```ez
list = List()
list.append(2).append(3)
list.prepend(1)
list.print()  // List[1 -> 2 -> 3]
```

---

##### `get(index)`
Returns value at index.

**Returns:** Value or `nil` if out of bounds

**Example:**
```ez
list = List()
list.append(10).append(20).append(30)
out list.get(0)  // 10
out list.get(1)  // 20
out list.get(5)  // nil (out of bounds)
```

---

##### `remove(index)`
Removes value at index.

**Returns:** Self (for chaining)

**Example:**
```ez
list = List()
list.append(1).append(2).append(3)
list.remove(1)  // Remove middle
list.print()  // List[1 -> 3]
```

---

##### `reverse()`
Reverses the list in-place.

**Returns:** Self (for chaining)

**Example:**
```ez
list = List()
list.append(1).append(2).append(3)
list.reverse()
list.print()  // List[3 -> 2 -> 1]
```

---

##### `size()`
Returns number of elements.

**Returns:** Number

**Example:**
```ez
list = List()
list.append(1).append(2)
out list.size()  // 2
```

---

##### `print()`
Prints list contents.

**Example:**
```ez
list.print()  // List[1 -> 2 -> 3]
```

---

### Counter - Frequency Counter

Counts occurrences of items. Perfect for histograms and frequency analysis.

#### Constructor
```ez
counter = Counter()
```

#### Methods

##### `add(item)`
Increments count for item.

**Returns:** Self (for chaining)

**Example:**
```ez
counter = Counter()
counter.add("apple")
       .add("banana")
       .add("apple")
       .add("apple")
```

---

##### `get(item)`
Returns count for item.

**Returns:** Number (0 if never added)

**Example:**
```ez
counter = Counter()
counter.add("A").add("A").add("B")
out counter.get("A")  // 2
out counter.get("B")  // 1
out counter.get("C")  // 0
```

---

##### `print()`
Prints all counts.

**Example:**
```ez
counter.print()  // Counter{apple: 3, banana: 1}
```

---

## 🎯 Complete Examples

### 1. Remove Duplicates

```ez
use "collections"

// Array with duplicates
numbers = [1, 2, 3, 2, 1, 4, 3, 5]

// Use Set to remove duplicates
uniqueSet = Set()
get num in numbers {
    uniqueSet.add(num)
}

out "Original: " + str(numbers)
out "Unique: " + str(uniqueSet.toArray())
out "Count: " + str(uniqueSet.size())
```

**Output:**
```
Original: [1, 2, 3, 2, 1, 4, 3, 5]
Unique: [1, 2, 3, 4, 5]
Count: 5
```

---

### 2. Word Frequency Counter

```ez
use "collections"

text = "the quick brown fox jumps over the lazy dog the fox"
words = split(text, " ")

counter = Counter()
get word in words {
    counter.add(word)
}

out "Word frequencies:"
get word in words {
    out word + ": " + str(counter.get(word))
}

// Most common word
counter.print()
```

**Output:**
```
Word frequencies:
the: 3
quick: 1
fox: 2
...
Counter{the: 3, quick: 1, brown: 1, fox: 2, ...}
```

---

### 3. Task Queue System

```ez
use "collections"

taskQueue = Queue()

// Producer: Add tasks
task addTask(name) {
    taskQueue.enqueue(name)
    out "Added task: " + name
}

// Consumer: Process tasks
task processTasks() {
    out "Processing tasks..."
    until taskQueue.isEmpty() {
        task = taskQueue.dequeue()
        out "Processing: " + task
        stop(500)  // Simulate work
    }
    out "All tasks completed!"
}

// Add tasks
addTask("Send email")
addTask("Generate report")
addTask("Backup database")

// Process them
processTasks()
```

**Output:**
```
Added task: Send email
Added task: Generate report
Added task: Backup database
Processing tasks...
Processing: Send email
Processing: Generate report
Processing: Backup database
All tasks completed!
```

---

### 4. Undo/Redo System

```ez
use "collections"

model Editor {
    init() {
        self.text = ""
        self.undoStack = Stack()
        self.redoStack = Stack()
    }
    
    task write(newText) {
        self.undoStack.push(self.text)
        self.text = newText
        self.redoStack = Stack()  // Clear redo
    }
    
    task undo() {
        when not self.undoStack.isEmpty() {
            self.redoStack.push(self.text)
            self.text = self.undoStack.pop()
        }
    }
    
    task redo() {
        when not self.redoStack.isEmpty() {
            self.undoStack.push(self.text)
            self.text = self.redoStack.pop()
        }
    }
    
    task display() {
        out "Text: " + self.text
    }
}

editor = Editor()
editor.write("Hello")
editor.display()  // Text: Hello

editor.write("Hello World")
editor.display()  // Text: Hello World

editor.undo()
editor.display()  // Text: Hello

editor.redo()
editor.display()  // Text: Hello World
```

---

### 5. Cache with Map

```ez
use "collections"

model Cache {
    init(maxSize) {
        self.cache = Map()
        self.maxSize = maxSize
    }
    
    task get(key) {
        when self.cache.has(key) {
            out "Cache HIT: " + key
            give self.cache.get(key)
        }
        out "Cache MISS: " + key
        give nil
    }
    
    task set(key, value) {
        when self.cache.size() >= self.maxSize {
            out "Cache full! Evicting..."
            // Simple eviction: remove first key
            keys = self.cache.keys()
            when keys.len > 0 {
                self.cache.remove(keys[0])
            }
        }
        self.cache.set(key, value)
    }
}

cache = Cache(3)
cache.set("user:1", "Alice")
cache.set("user:2", "Bob")
cache.set("user:3", "Charlie")

out cache.get("user:1")  // Cache HIT: user:1
                         // Alice

cache.set("user:4", "David")  // Cache full! Evicting...

out cache.get("user:1")  // Cache MISS: user:1 (evicted)
                         // nil
```

---

### 6. Graph Traversal (BFS with Queue)

```ez
use "collections"

task bfs(graph, start) {
    visited = Set()
    queue = Queue()
    
    queue.enqueue(start)
    visited.add(start)
    
    out "BFS Traversal from " + start + ":"
    
    until queue.isEmpty() {
        node = queue.dequeue()
        out "Visiting: " + node
        
        // Visit neighbors
        when graph.has(node) {
            neighbors = graph.get(node)
            get neighbor in neighbors {
                when not visited.has(neighbor) {
                    visited.add(neighbor)
                    queue.enqueue(neighbor)
                }
            }
        }
    }
}

// Build graph
graph = Map()
graph.set("A", ["B", "C"])
graph.set("B", ["D", "E"])
graph.set("C", ["F"])
graph.set("D", [])
graph.set("E", [])
graph.set("F", [])

bfs(graph, "A")
```

**Output:**
```
BFS Traversal from A:
Visiting: A
Visiting: B
Visiting: C
Visiting: D
Visiting: E
Visiting: F
```

---

### 7. Graph Traversal (DFS with Stack)

```ez
use "collections"

task dfs(graph, start) {
    visited = Set()
    stack = Stack()
    
    stack.push(start)
    
    out "DFS Traversal from " + start + ":"
    
    until stack.isEmpty() {
        node = stack.pop()
        
        when not visited.has(node) {
            visited.add(node)
            out "Visiting: " + node
            
            // Push neighbors
            when graph.has(node) {
                neighbors = graph.get(node)
                get neighbor in neighbors {
                    when not visited.has(neighbor) {
                        stack.push(neighbor)
                    }
                }
            }
        }
    }
}

// Same graph as BFS example
graph = Map()
graph.set("A", ["B", "C"])
graph.set("B", ["D", "E"])
graph.set("C", ["F"])
graph.set("D", [])
graph.set("E", [])
graph.set("F", [])

dfs(graph, "A")
```

**Output:**
```
DFS Traversal from A:
Visiting: A
Visiting: C
Visiting: F
Visiting: B
Visiting: E
Visiting: D
```

---

### 8. LRU Cache (Least Recently Used)

```ez
use "collections"

model LRUCache {
    init(capacity) {
        self.capacity = capacity
        self.cache = Map()
        self.order = List()
    }
    
    task get(key) {
        when not self.cache.has(key) {
            give nil
        }
        
        // Move to end (most recently used)
        self.removeFromOrder(key)
        self.order.append(key)
        
        give self.cache.get(key)
    }
    
    task set(key, value) {
        when self.cache.has(key) {
            // Update existing
            self.cache.set(key, value)
            self.removeFromOrder(key)
            self.order.append(key)
        } other {
            // Add new
            when self.cache.size() >= self.capacity {
                // Evict least recently used (first in list)
                oldest = self.order.get(0)
                self.cache.remove(oldest)
                self.order.remove(0)
                out "Evicted: " + oldest
            }
            self.cache.set(key, value)
            self.order.append(key)
        }
    }
    
    task removeFromOrder(key) {
        i = 0
        until i >= self.order.size() {
            when self.order.get(i) == key {
                self.order.remove(i)
                give 0
            }
            i = i + 1
        }
    }
}

cache = LRUCache(3)
cache.set("a", 1)
cache.set("b", 2)
cache.set("c", 3)
out cache.get("a")  // 1 (move 'a' to end)
cache.set("d", 4)   // Evicted: b (least recent)
out cache.get("b")  // nil (was evicted)
```

---

## 🎨 Design Patterns

### 1. Set Operations

```ez
use "collections"

task union(set1, set2) {
    result = Set()
    get val in set1.toArray() { result.add(val) }
    get val in set2.toArray() { result.add(val) }
    give result
}

task intersection(set1, set2) {
    result = Set()
    get val in set1.toArray() {
        when set2.has(val) {
            result.add(val)
        }
    }
    give result
}

task difference(set1, set2) {
    result = Set()
    get val in set1.toArray() {
        when not set2.has(val) {
            result.add(val)
        }
    }
    give result
}

// Usage
a = Set()
a.add(1).add(2).add(3)

b = Set()
b.add(2).add(3).add(4)

union(a, b).print()         // Set{1, 2, 3, 4}
intersection(a, b).print()  // Set{2, 3}
difference(a, b).print()    // Set{1}
```

---

### 2. Priority Queue (Simple)

```ez
use "collections"

model PriorityQueue {
    init() {
        self.items = List()
    }
    
    task enqueue(value, priority) {
        item = {value: value, priority: priority}
        self.items.append(item)
    }
    
    task dequeue() {
        when self.items.size() == 0 {
            give nil
        }
        
        // Find highest priority (lowest number)
        minIdx = 0
        minPriority = self.items.get(0)["priority"]
        
        i = 1
        until i >= self.items.size() {
            current = self.items.get(i)
            when current["priority"] < minPriority {
                minIdx = i
                minPriority = current["priority"]
            }
            i = i + 1
        }
        
        item = self.items.get(minIdx)
        self.items.remove(minIdx)
        give item["value"]
    }
}

pq = PriorityQueue()
pq.enqueue("Low priority task", 10)
pq.enqueue("High priority task", 1)
pq.enqueue("Medium priority task", 5)

out pq.dequeue()  // High priority task (1)
out pq.dequeue()  // Medium priority task (5)
out pq.dequeue()  // Low priority task (10)
```

---

## 📊 Performance Characteristics

### Time Complexity

| Operation | Set | Map | Queue | Stack | List | Counter |
|-----------|-----|-----|-------|-------|------|---------|
| Insert | O(1) | O(1) | O(1) | O(1) | O(n)* | O(1) |
| Remove | O(1) | O(1) | O(n)** | O(1) | O(n) | O(1) |
| Search | O(1) | O(1) | O(n) | O(n) | O(n) | O(1) |
| Access | - | O(1) | O(1)*** | O(1)*** | O(1) | O(1) |

\* `append()` is O(1), `prepend()` is O(n)  
\*\* `dequeue()` is O(n) due to array shift  
\*\*\* `peek()` only

### Space Complexity
All structures: **O(n)** where n is number of elements

---

## 💡 Best Practices

### 1. Choose the Right Structure

```ez
// ✅ Use Set for unique values
uniqueIds = Set()
uniqueIds.add(userId)

// ✅ Use Map for lookups
userCache = Map()
userCache.set(userId, userData)

// ✅ Use Queue for task processing
taskQueue = Queue()
taskQueue.enqueue(newTask)

// ✅ Use Stack for undo/redo
undoStack = Stack()
undoStack.push(previousState)

// ✅ Use Counter for frequencies
wordCount = Counter()
wordCount.add(word)
```

---

### 2. Method Chaining

```ez
// ✅ Good - Clean chaining
set = Set()
set.add(1)
   .add(2)
   .add(3)

// ❌ Avoid - Verbose
set = Set()
set.add(1)
set.add(2)
set.add(3)
```

---

### 3. Check Before Operations

```ez
// ✅ Good - Check before dequeue
when not queue.isEmpty() {
    item = queue.dequeue()
    process(item)
}

// ❌ Avoid - May get nil
item = queue.dequeue()  // nil if empty!
```

---

### 4. Use Appropriate Methods

```ez
// ✅ Good - Use has() for membership
when mySet.has(value) {
    // value exists
}

// ❌ Avoid - Converting unnecessarily
when value in mySet.toArray() {
    // Slower - creates array first
}
```

---

## 🐛 Common Issues

### Issue: Queue dequeue is slow
**Cause:** O(n) operation due to array shift  
**Solution:** Use for small queues, or implement circular buffer for high-performance needs

### Issue: Set/Map order not guaranteed
**Cause:** Hash-based storage  
**Solution:** Use List if order matters, then use Set for uniqueness

### Issue: Counter returns 0 for missing items
**Expected:** This is correct behavior  
**Solution:** Check with `get()` - returns 0 if never added

---

## 📚 Real-World Use Cases

### Web Scraping - Visited URLs
```ez
visited = Set()

task crawl(url) {
    when visited.has(url) {
        give 0  // Already visited
    }
    visited.add(url)
    // ... crawl logic
}
```

### Caching - User Sessions
```ez
sessions = Map()

task getSession(userId) {
    when sessions.has(userId) {
        give sessions.get(userId)
    }
    // Create new session
    session = createSession(userId)
    sessions.set(userId, session)
    give session
}
```

### Task Processing - Job Queue
```ez
jobQueue = Queue()

task scheduleJob(job) {
    jobQueue.enqueue(job)
}

task processJobs() {
    until jobQueue.isEmpty() {
        job = jobQueue.dequeue()
        executeJob(job)
    }
}
```

---

## 🔗 Related Libraries

- **db** - Store collections in database
- **csv** - Export Map/Counter to CSV
- **json** - Serialize collections
- **gui** - Use ListView with List

---

## 📄 License

MIT License - Free for any use

## 🤝 Contributing

Found a bug? Want to add a data structure?  
Contributions welcome!

---

Made with ❤️ for the EZ Language Community

**Professional data structures for professional applications** 🚀
