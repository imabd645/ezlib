# EZ ORM - Active Record for EZ Language

EZ ORM is a lightweight, intuitive, and modern Object-Relational Mapping (ORM) library built completely in EZ. Designed closely around the Active Record pattern, it abstracts away raw SQL queries into beautiful chainable methods for interacting with SQLite databases natively.

## Getting Started

Include the ORM library in your EZ project:
```bash
ez install orm
```
```ez
use "orm"
```

### Initializing the Database

To start using EZ ORM, initialize a `Database` instance with the path to your SQLite database file. It will automatically be created if it doesn't exist.

```ez
db = Database("app.db")
```

### Defining a Model

The `Database.define(tableName, schema)` method creates a new table if it does not already exist, and returns a usable `Table` model instance to perform operations.

*By default, the ORM automatically adds an `id INTEGER PRIMARY KEY AUTOINCREMENT` field to every table.*

```ez
User = db.define("users", {
    "name": "TEXT",
    "email": "TEXT",
    "age": "INTEGER",
    "is_active": "INTEGER"
})
```

---

## CRUD Operations

The ORM supports standard CRUD lifecycle methods natively using EZ dictionaries `{"key": "value"}`.

### Create (Insert)

To add a new row to the table, pass a dictionary mapping columns to values using the `insert()` method. It returns the `id` of the newly created row.

```ez
newUserId = User.insert({
    "name": "Abdullah",
    "email": "abdullah@ezlang.org",
    "age": 21,
    "is_active": 1
})
```

### Read (Find, FindOne, FindById, All)

Retrieve records matching specific conditions.

**1. `User.find(conditions)`**  
Returns a list (array) of all rows matching the dictionary conditions.
```ez
# Find all active users
workingUsers = User.find({"is_active": 1})
```

**2. `User.findOne(conditions)`**  
Returns a single dictionary for the first matching row, or `nil` if not found.
```ez
# Find one specific user by email
admin = User.findOne({"email": "admin@example.com"})
```

**3. `User.findById(id)`**  
Quickly retrieve a unique row by its Primary Key `id`.
```ez
# Retrieve the user we just inserted
firstUser = User.findById(newUserId)
```

**4. `User.all()`**  
Returns a list of every row in the table.
```ez
allDocs = User.all()
```

### Update

To update a row, call `update(id, data)` passing the record's primary key `id` and a dictionary of the fields you want mapped to their new values.

```ez
# Update Abdullah's age to 22
User.update(newUserId, {"age": 22})
```

### Delete

Remove rows from the database.

**1. `User.delete(id)`**
Deletes a single row matching the exact primary key `id`.
```ez
User.delete(newUserId)
```

**2. `User.deleteWhere(conditions)`**
Deletes all rows that match the provided condition dictionary.
```ez
# Clear out all inactive users
User.deleteWhere({"is_active": 0})
```

---

## Utility Methods

- **`User.count()`** — Returns the total number of rows in the table.
- **`db.execute(query)`** — Run a raw SQL operation.
- **`db.query(query)`** — Run a raw SQL `SELECT` statement and get back an EZ Array of Dictionaries.
- **`db.begin()`, `db.commit()`, `db.rollback()`** — Advanced transaction controls.
- **`db.close()`** — Closes the active local SQLite connection safely.

---

## Example Program

```ez
use "orm"

db = Database("store.db")

Product = db.define("products", {
    "title": "TEXT",
    "price": "REAL",
    "stock": "INTEGER"
})

# Add an item
Product.insert({
    "title": "Mechanical Keyboard",
    "price": 129.99,
    "stock": 50
})

out "Total products: " + str(Product.count())

# Query an item
p = Product.findOne({"title": "Mechanical Keyboard"})
out "Found " + p["title"] + " costing $" + str(p["price"])

# Buy one!
Product.update(p["id"], {"stock": p["stock"] - 1})

db.close()
```
