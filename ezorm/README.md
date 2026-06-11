# ezorm — Object-Relational Mapping Library for EZ

> **Version:** 1.0  
> **Import:** `use "ezorm"`  
> **File:** `E:\ezlib\ezorm\main.ez`

---

## Overview

`ezorm` is a lightweight **Active-Record style ORM** for EZ, wrapping SQLite through EZ built-in `db_*` functions. It provides a clean, model-driven API for database operations without writing raw SQL for common CRUD operations.

---

## Quick Start

```ez
use "ezorm"

db = Database("myapp.db")

# Define a table (auto-creates with schema)
users = db.define("users", {
    "name": "TEXT NOT NULL",
    "email": "TEXT UNIQUE",
    "age": "INTEGER"
})

# Insert
id = users.insert({"name": "Alice", "email": "alice@example.com", "age": 30})

# Find all
all = users.all()
out len(all)  # → 1

# Find by condition
found = users.find({"name": "Alice"})
out found[0]["email"]  # → "alice@example.com"

# Update
users.update(id, {"age": 31})

# Delete
users.delete(id)
```

---

## Model: `Database`

### `Database(path)`
Opens or creates a SQLite database file using the `db_open()` built-in.

**Parameters:**
- `path` — File path to the `.db` or `.sqlite` file. Relative or absolute.

```ez
use "ezorm"

db = Database("store.db")
db = Database(":memory:")   # In-memory
```

---

### `db.execute(query)` → result
Executes raw SQL (no parameterization).

```ez
use "ezorm"

db = Database(":memory:")
db.execute("CREATE TABLE IF NOT EXISTS logs (msg TEXT)")
db.execute("INSERT INTO logs VALUES ('hello')")
```

---

### `db.query(query)` → `array`
Executes a raw SELECT and returns array of row dictionaries.

```ez
use "ezorm"

db = Database(":memory:")
db.execute("CREATE TABLE t (x INTEGER)")
db.execute("INSERT INTO t VALUES (1)")
db.execute("INSERT INTO t VALUES (2)")

rows = db.query("SELECT * FROM t WHERE x > 0")
out rows[0]["x"]  # → 1
```

---

### `db.lastInsertId()` → `number`
Returns the auto-generated ID of the last INSERT.

```ez
use "ezorm"

db = Database(":memory:")
db.execute("CREATE TABLE t (id INTEGER PRIMARY KEY, name TEXT)")
db.execute("INSERT INTO t (name) VALUES ('first')")
out db.lastInsertId()  # → 1
```

---

### `db.define(tableName, schema)` → `Table`

Defines a table and returns a `Table` object for CRUD operations. Automatically creates the table with `CREATE TABLE IF NOT EXISTS`.

**Parameters:**
- `tableName` — The SQL table name string.
- `schema` — A dictionary mapping column names to their SQLite type strings.

**Auto-ID:** If your schema does NOT include an `"id"` key, the library automatically prepends `id INTEGER PRIMARY KEY AUTOINCREMENT`.

```ez
use "ezorm"

db = Database(":memory:")

# Schema WITHOUT id — auto-adds AUTOINCREMENT id
posts = db.define("posts", {
    "title": "TEXT NOT NULL",
    "body": "TEXT",
    "published": "INTEGER DEFAULT 0"
})

# Schema WITH custom id
custom = db.define("events", {
    "id": "TEXT PRIMARY KEY",   # custom UUID-based key
    "name": "TEXT"
})
```

---

### Transactions

```ez
use "ezorm"

db = Database(":memory:")

db.begin()
try {
    db.execute("INSERT INTO t VALUES (1)")
    db.execute("INSERT INTO t VALUES (2)")
    db.commit()
} catch e {
    db.rollback()
    out "Transaction failed: " + e
}
```

---

### `db.close()`
Closes the database connection.

```ez
use "ezorm"

db = Database("app.db")
# ... work ...
db.close()
```

---

## Model: `Table`

A `Table` provides high-level CRUD operations against one SQL table.

### `Table(db, tableName, schema)`
Created via `db.define()` — do not instantiate directly.

---

### `table.insert(data)` → `number`

Inserts a new row and returns the new row's `id`.

**Parameters:**
- `data` — Dictionary of column name → value pairs. String values are single-quote escaped.

```ez
use "ezorm"

db = Database(":memory:")
products = db.define("products", {
    "name": "TEXT",
    "price": "REAL",
    "stock": "INTEGER"
})

id1 = products.insert({"name": "Widget", "price": 9.99, "stock": 100})
id2 = products.insert({"name": "Gadget", "price": 24.99, "stock": 50})

out id1   # → 1
out id2   # → 2
```

---

### `table.find(conditions)` → `array`

Finds all rows matching the given conditions. Conditions are ANDed together.

**Parameters:**
- `conditions` — Dictionary of column → value pairs, or `nil`/`false` for all rows.

```ez
use "ezorm"

db = Database(":memory:")
users = db.define("users", {"name": "TEXT", "role": "TEXT", "active": "INTEGER"})

users.insert({"name": "Alice", "role": "admin", "active": 1})
users.insert({"name": "Bob", "role": "user", "active": 1})
users.insert({"name": "Charlie", "role": "user", "active": 0})

# Find all active users
activeUsers = users.find({"active": 1})
out len(activeUsers)  # → 2

# Find admins only
admins = users.find({"role": "admin"})
out admins[0]["name"]  # → "Alice"

# Multiple conditions (AND)
activeAdmins = users.find({"role": "admin", "active": 1})
out len(activeAdmins)  # → 1
```

---

### `table.findOne(conditions)` → `dictionary | nil`

Finds the first matching row. Returns `nil` if no match.

```ez
use "ezorm"

db = Database(":memory:")
users = db.define("users", {"name": "TEXT", "email": "TEXT"})
users.insert({"name": "Alice", "email": "alice@example.com"})

user = users.findOne({"name": "Alice"})
when user != nil {
    out user["email"]   # → "alice@example.com"
}

missing = users.findOne({"name": "Nobody"})
out missing   # → nil
```

---

### `table.findById(id)` → `dictionary | nil`

Shorthand for `findOne({"id": id})`.

```ez
use "ezorm"

db = Database(":memory:")
items = db.define("items", {"label": "TEXT"})

id = items.insert({"label": "First"})
row = items.findById(id)

out row["label"]   # → "First"
out items.findById(9999)  # → nil
```

---

### `table.update(id, data)` → `self`

Updates a row by its `id`.

**Parameters:**
- `id` — The integer row ID.
- `data` — Dictionary of columns to update.

```ez
use "ezorm"

db = Database(":memory:")
products = db.define("products", {"name": "TEXT", "price": "REAL"})

id = products.insert({"name": "Widget", "price": 9.99})
products.update(id, {"price": 12.50})

row = products.findById(id)
out row["price"]   # → 12.5
```

---

### `table.delete(id)` → `self`

Deletes the row with the given `id`.

```ez
use "ezorm"

db = Database(":memory:")
users = db.define("users", {"name": "TEXT"})

id = users.insert({"name": "TempUser"})
out users.count()   # → 1

users.delete(id)
out users.count()   # → 0
```

---

### `table.deleteWhere(conditions)` → `self`

Deletes all rows matching the conditions. If `conditions` is empty or nil, nothing is deleted.

```ez
use "ezorm"

db = Database(":memory:")
logs = db.define("logs", {"level": "TEXT", "msg": "TEXT"})

logs.insert({"level": "debug", "msg": "trace 1"})
logs.insert({"level": "debug", "msg": "trace 2"})
logs.insert({"level": "error", "msg": "critical failure"})

logs.deleteWhere({"level": "debug"})
out logs.count()   # → 1
```

---

### `table.all()` → `array`

Returns all rows in the table with no filtering.

```ez
use "ezorm"

db = Database(":memory:")
items = db.define("items", {"val": "INTEGER"})

items.insert({"val": 1})
items.insert({"val": 2})
items.insert({"val": 3})

rows = items.all()
out len(rows)   # → 3
```

---

### `table.count()` → `number`

Returns the total number of rows in the table.

```ez
use "ezorm"

db = Database(":memory:")
t = db.define("t", {"x": "INTEGER"})

t.insert({"x": 1})
t.insert({"x": 2})
out t.count()  # → 2
```

---

## Edge Cases & Important Notes

### String Escaping (No Parameterization)
`ezorm` builds SQL by string concatenation using `_escape()`. Strings are wrapped in single quotes. This is **not safe against SQL injection** if untrusted input is inserted. For user-provided data, sanitize manually or use `ezdb` with proper parameterized queries.

```ez
# ⚠️ Vulnerable if 'name' contains SQL special characters
users.insert({"name": userInput})

# Current _escape() behavior:
# string → 'value' (single-quoted)
# nil    → NULL
# other  → raw str(val)
```

### Auto-ID Column
The ORM assumes every table has an `id` INTEGER column. All `findById()`, `update(id)`, and `delete(id)` calls reference `WHERE id = ?`. If you define a custom `id` column with a different type (e.g. TEXT), ensure compatibility.

### No Migrations
`CREATE TABLE IF NOT EXISTS` only creates the table if it doesn't exist. Changing schema after the table is created requires manual `ALTER TABLE` calls via `db.execute()`.

### Schema Keys Are Unordered
EZ dictionaries don't guarantee key order. The column order in `CREATE TABLE` may vary. This is fine for SQLite (which is column-name-based, not position-based) but keep it in mind.

### `db_*` Built-ins Required
This ORM uses `db_open`, `db_execute`, `db_query`, `db_close`, `db_last_insert_id`, `db_begin`, `db_commit`, `db_rollback` — all EZ built-in functions backed by an embedded SQLite binding in the EZ runtime. If your EZ binary doesn't include these, use `ezdb` (FFI-based) instead.

---

## Full Example: Blog System

```ez
use "ezorm"

db = Database("blog.db")

posts = db.define("posts", {
    "title": "TEXT NOT NULL",
    "body": "TEXT",
    "author": "TEXT",
    "published": "INTEGER DEFAULT 0",
    "views": "INTEGER DEFAULT 0"
})

comments = db.define("comments", {
    "post_id": "INTEGER",
    "author": "TEXT",
    "body": "TEXT"
})

# Create posts
id1 = posts.insert({"title": "Hello EZ", "body": "Welcome to EZ blog", "author": "alice", "published": 1, "views": 0})
id2 = posts.insert({"title": "Draft Post", "body": "Work in progress...", "author": "bob", "published": 0, "views": 0})

# Add comments to post 1
comments.insert({"post_id": id1, "author": "charlie", "body": "Great post!"})
comments.insert({"post_id": id1, "author": "dave", "body": "Learned a lot."})

# Increment view count
post = posts.findById(id1)
posts.update(id1, {"views": post["views"] + 1})

# Get published posts
published = posts.find({"published": 1})
out "Published posts: " + str(len(published))

get p in published {
    postComments = comments.find({"post_id": p["id"]})
    out "\n[" + p["title"] + "] by " + p["author"]
    out "  Views: " + str(p["views"])
    out "  Comments: " + str(len(postComments))
    get c in postComments {
        out "    - " + c["author"] + ": " + c["body"]
    }
}

db.close()
```

---

## Full Example: Inventory Management

```ez
use "ezorm"

db = Database("inventory.db")

items = db.define("items", {
    "sku": "TEXT UNIQUE",
    "name": "TEXT",
    "quantity": "INTEGER",
    "price": "REAL"
})

# Stock initial inventory
items.insert({"sku": "WID-001", "name": "Widget", "quantity": 500, "price": 9.99})
items.insert({"sku": "GAD-002", "name": "Gadget", "quantity": 200, "price": 24.99})
items.insert({"sku": "DON-003", "name": "Donut", "quantity": 50, "price": 1.99})

task restockItem(sku, addQty) {
    item = items.findOne({"sku": sku})
    when item == nil {
        out "Item not found: " + sku
        give
    }
    items.update(item["id"], {"quantity": item["quantity"] + addQty})
    out "Restocked " + sku + " by " + str(addQty) + " units"
}

task sellItem(sku, sellQty) {
    item = items.findOne({"sku": sku})
    when item == nil { out "Item not found: " + sku   give }
    when item["quantity"] < sellQty {
        out "Insufficient stock: only " + str(item["quantity"]) + " left"
        give
    }
    items.update(item["id"], {"quantity": item["quantity"] - sellQty})
    out "Sold " + str(sellQty) + " x " + item["name"]
}

sellItem("WID-001", 30)
restockItem("DON-003", 100)

out "\nCurrent Inventory:"
all = items.all()
get item in all {
    out "  " + item["sku"] + " | " + item["name"] + " | qty: " + str(item["quantity"]) + " | $" + str(item["price"])
}

db.close()
```

---

*Documentation generated from `E:\ezlib\ezorm\main.ez` — EZ ORM Library*
