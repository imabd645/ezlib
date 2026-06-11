# ezdb — SQLite Database Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezdb"`  
> **File:** `E:\ezlib\ezdb\main.ez`  
> **Requires:** `sqlite3.dll` or `libsqlite3-0.dll` (Windows)

---

## Overview

`ezdb` is a production-grade SQLite3 wrapper for EZ, built entirely through FFI (Foreign Function Interface) without any C++ bindings. It provides:

- Database open/close
- Prepared statements with parameter binding
- SELECT queries returning typed rows
- INSERT, UPDATE, DELETE execution
- BLOB and NULL support
- Transactions (BEGIN / COMMIT / ROLLBACK)
- Scalar result helper
- Row ID and change count access

---

## Quick Start

```ez
use "ezdb"

db = Database("myapp.db")

# Create a table
db.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)", nil)

# Insert
db.execute("INSERT INTO users (name, age) VALUES (?, ?)", ["Alice", 30])

# Query
rows = db.query("SELECT * FROM users WHERE age > ?", [25])
get row in rows {
    out row["name"] + " is " + str(row["age"])
}

db.close()
```

---

## Installation / Requirements

`ezdb` requires `sqlite3.dll` (or `libsqlite3-0.dll`) to be accessible. The library searches for it in this order:

1. `sqlite3.dll` (system PATH or working directory)
2. `libsqlite3-0.dll` (common MinGW/MSYS2 name)
3. `dlls\libsqlite3-0.dll` (relative subdirectory)

If none are found, the constructor throws an error.

---

## Model: `Database`

The primary interface. One `Database` object represents one open SQLite database connection.

### `Database(path)`

Opens a SQLite database file. Creates it if it doesn't exist.

**Parameters:**
- `path` — File path to the `.db` or `.sqlite` file. Use `":memory:"` for an in-memory database.

**Throws:** String error if the DLL is not found or the database cannot be opened.

```ez
use "ezdb"

# File database
db = Database("C:\\data\\app.db")

# In-memory database (fastest, non-persistent)
memDb = Database(":memory:")
```

---

### `db.execute(sql, params)` → `true`

Executes a SQL statement that does NOT return rows (INSERT, UPDATE, DELETE, CREATE, DROP, etc.).

**Parameters:**
- `sql` — SQL statement with `?` placeholders.
- `params` — Array of parameter values, or `nil` if no parameters.

**Returns:** `true` on success.

**Throws:** SQL error string on failure.

```ez
use "ezdb"

db = Database(":memory:")

# Create table
db.execute("CREATE TABLE products (id INTEGER PRIMARY KEY, name TEXT, price REAL)", nil)

# Insert with params
db.execute("INSERT INTO products (name, price) VALUES (?, ?)", ["Widget", 9.99])
db.execute("INSERT INTO products (name, price) VALUES (?, ?)", ["Gadget", 24.99])

# Update
db.execute("UPDATE products SET price = ? WHERE name = ?", [12.50, "Widget"])

# Delete
db.execute("DELETE FROM products WHERE price > ?", [20.0])
```

---

### `db.query(sql, params)` → `array`

Executes a SELECT statement and returns all matching rows.

**Parameters:**
- `sql` — SELECT SQL with `?` placeholders.
- `params` — Array of values, or `nil`.

**Returns:** Array of dictionaries. Each dictionary maps column names to their values. Types are automatically inferred:
- `INTEGER` → EZ integer
- `REAL` → EZ float
- `TEXT` → EZ string
- `BLOB` → EZ buffer (binary)
- `NULL` → `nil`

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE t (id INTEGER, val TEXT, score REAL)", nil)
db.execute("INSERT INTO t VALUES (1, 'hello', 3.14)", nil)
db.execute("INSERT INTO t VALUES (2, 'world', 2.71)", nil)

rows = db.query("SELECT * FROM t", nil)
out len(rows)       # → 2
out rows[0]["id"]   # → 1
out rows[0]["val"]  # → "hello"
out rows[1]["score"]# → 2.71

# Filtered query
filtered = db.query("SELECT val FROM t WHERE score > ?", [3.0])
out filtered[0]["val"]  # → "hello"
```

---

### `db.fetchScalar(sql, params)` → `value | nil`

Executes a query and returns the first column of the first row only. Ideal for COUNT, SUM, single-value lookups.

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE scores (val INTEGER)", nil)
db.execute("INSERT INTO scores VALUES (10)", nil)
db.execute("INSERT INTO scores VALUES (20)", nil)
db.execute("INSERT INTO scores VALUES (30)", nil)

total = db.fetchScalar("SELECT SUM(val) FROM scores", nil)
out total   # → 60

count = db.fetchScalar("SELECT COUNT(*) FROM scores", nil)
out count   # → 3

specific = db.fetchScalar("SELECT val FROM scores WHERE val = ?", [20])
out specific  # → 20

missing = db.fetchScalar("SELECT val FROM scores WHERE val = ?", [999])
out missing   # → nil
```

---

### `db.lastRowId()` → `number`

Returns the row ID of the most recently inserted row.

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT)", nil)
db.execute("INSERT INTO items (name) VALUES (?)", ["First"])
out db.lastRowId()  # → 1

db.execute("INSERT INTO items (name) VALUES (?)", ["Second"])
out db.lastRowId()  # → 2
```

---

### `db.changes()` → `number`

Returns the number of rows modified by the last INSERT, UPDATE, or DELETE.

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE t (val INTEGER)", nil)
db.execute("INSERT INTO t VALUES (1)", nil)
db.execute("INSERT INTO t VALUES (2)", nil)
db.execute("INSERT INTO t VALUES (3)", nil)

db.execute("DELETE FROM t WHERE val > ?", [1])
out db.changes()  # → 2
```

---

### `db.prepareStatement(sql)` → `DbStatement`

Creates a compiled, reusable `DbStatement` object. Use this for performance when executing the same statement many times with different parameters.

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE logs (msg TEXT, level INTEGER)", nil)

stmt = db.prepareStatement("INSERT INTO logs (msg, level) VALUES (?, ?)")

repeat i = 0 to 99 {
    stmt.bind(["Log entry " + str(i), 1])
    stmt.step()
    stmt.reset()
    stmt.clearBindings()
}

stmt.finalize()
out db.fetchScalar("SELECT COUNT(*) FROM logs", nil)  # → 100
```

---

## Transactions

### `db.begin()`
Starts a database transaction. Changes are not committed until `commit()` is called.

### `db.commit()`
Commits the current transaction, making all changes permanent.

### `db.rollback()`
Rolls back the current transaction, discarding all changes since `begin()`.

### `db.transaction(actionsFn)`
Convenience method. Runs `actionsFn()` inside a transaction. Automatically commits on success, rolls back and re-throws on error.

```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE accounts (id INTEGER, balance REAL)", nil)
db.execute("INSERT INTO accounts VALUES (1, 1000.0)", nil)
db.execute("INSERT INTO accounts VALUES (2, 500.0)", nil)

# Safe transfer using transaction
task transfer(fromId, toId, amount) {
    db.transaction(|| {
        db.execute("UPDATE accounts SET balance = balance - ? WHERE id = ?", [amount, fromId])
        db.execute("UPDATE accounts SET balance = balance + ? WHERE id = ?", [amount, toId])
    })
}

transfer(1, 2, 200.0)

rows = db.query("SELECT * FROM accounts", nil)
out rows[0]["balance"]  # → 800.0
out rows[1]["balance"]  # → 700.0
```

**Manual Transaction Example (with error handling):**
```ez
use "ezdb"

db = Database(":memory:")
db.execute("CREATE TABLE t (val INTEGER)", nil)

db.begin()
try {
    db.execute("INSERT INTO t VALUES (?)", [1])
    db.execute("INSERT INTO t VALUES (?)", [2])
    # Simulate failure
    throw "Something went wrong!"
    db.commit()
} catch e {
    db.rollback()
    out "Transaction rolled back: " + e
}

out db.fetchScalar("SELECT COUNT(*) FROM t", nil)  # → 0 (rolled back)
```

---

## Model: `DbStatement`

A compiled SQL statement that can be bound, stepped, and reset. Returned by `db.prepareStatement()`.

### `stmt.bind(params)` → `self`

Binds an array of parameters to the `?` placeholders.

- **string** → `sqlite3_bind_text`
- **integer** → `sqlite3_bind_int64`
- **float** → `sqlite3_bind_double`
- **buffer** → `sqlite3_bind_blob`
- **nil** → `sqlite3_bind_null`

```ez
stmt.bind(["hello", 42, 3.14, nil])
```

### `stmt.step()` → `boolean`
Advances the statement by one row.
- Returns `true` if a row is available (SQLITE_ROW = 100)
- Returns `false` when done (SQLITE_DONE = 101)
- Throws on error

### `stmt.getRow()` → `dictionary`
Returns the current row as a key-value dictionary (must call after `step()` returned `true`).

### `stmt.reset()` → `self`
Resets the statement to the beginning for re-execution with new parameters.

### `stmt.clearBindings()` → `self`
Clears all bound parameter values (resets to NULL).

### `stmt.finalize()`
Destroys the statement and frees SQLite memory. **Always call this when done.**

---

## Edge Cases & Important Notes

### Always Finalize Statements
Unreleased prepared statements will hold file locks on the database. Always call `stmt.finalize()` when done.

### Parameterized Queries vs String Interpolation
Always use `?` placeholders — **never** build SQL by string concatenation. SQL injection via string building:
```ez
# ❌ DANGEROUS — SQL injection possible
name = "Robert'); DROP TABLE users;--"
db.execute("SELECT * FROM t WHERE name = '" + name + "'", nil)

# ✅ SAFE — use parameterized queries
db.query("SELECT * FROM t WHERE name = ?", [name])
```

### Float Binding
Floats are bound via the signature-aware `os_call_sig` with `"iif"` signature to ensure correct ABI calling convention on Windows x64.

### In-Memory Databases
`:memory:` databases are private to one connection and are destroyed when `db.close()` is called or the script exits.

### Multiple Connections
EZ does not have a connection pool. Each `Database()` call opens a new connection. For concurrent access from multiple threads, use separate `Database` instances or the EZ `thread` library with careful synchronization.

### NULL Handling
Columns with NULL values come back as EZ `nil`. Check with `val == nil` before use.

### BLOB Data
BLOB columns are returned as EZ `buffer` objects created by `os_buffer_from_ptr`. Use buffer operations to read their contents.

---

## Full Example: User Authentication System

```ez
use "ezdb"
use "ezcrypto"

db = Database("auth.db")

# Schema
db.execute("CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    created_at TEXT
)", nil)

task registerUser(username, password) {
    salt = "ez-salt-2025"
    hash = Crypto.rc4Encrypt(salt, password)
    
    try {
        db.execute("INSERT INTO users (username, password_hash, created_at) VALUES (?, ?, ?)", 
            [username, hash, "2025-06-11"])
        out "User registered: " + username
        give db.lastRowId()
    } catch e {
        out "Registration failed: " + e
        give nil
    }
}

task loginUser(username, password) {
    salt = "ez-salt-2025"
    hash = Crypto.rc4Encrypt(salt, password)
    
    row = db.fetchScalar("SELECT id FROM users WHERE username = ? AND password_hash = ?",
        [username, hash])
    
    when row != nil {
        out "Login successful! User ID: " + str(row)
        give row
    }
    out "Invalid username or password."
    give nil
}

registerUser("alice", "password123")
registerUser("bob", "securepass")

loginUser("alice", "password123")   # → Login successful!
loginUser("alice", "wrongpassword") # → Invalid username or password.

db.close()
```

---

## Full Example: Product Catalog

```ez
use "ezdb"

db = Database(":memory:")

db.execute("CREATE TABLE products (
    id INTEGER PRIMARY KEY,
    name TEXT,
    category TEXT,
    price REAL,
    stock INTEGER
)", nil)

products = [
    ["Laptop", "Electronics", 999.99, 15],
    ["Mouse", "Electronics", 29.99, 200],
    ["Desk", "Furniture", 349.99, 8],
    ["Chair", "Furniture", 199.99, 20]
]

get p in products {
    db.execute("INSERT INTO products (name, category, price, stock) VALUES (?, ?, ?, ?)", p)
}

# Get all electronics under $100
cheap = db.query("SELECT name, price FROM products WHERE category = ? AND price < ? ORDER BY price", 
    ["Electronics", 100.0])
    
out "Cheap electronics:"
get item in cheap {
    out "  " + item["name"] + " - $" + str(item["price"])
}

# Count by category
categories = db.query("SELECT category, COUNT(*) as count, AVG(price) as avg_price FROM products GROUP BY category", nil)
get cat in categories {
    out cat["category"] + ": " + str(cat["count"]) + " items, avg $" + str(cat["avg_price"])
}

db.close()
```

---

*Documentation generated from `E:\ezlib\ezdb\main.ez` — EZ SQLite Database Library*
