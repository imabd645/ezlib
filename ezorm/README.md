# EZ ORM

A lightweight, SQLAlchemy-inspired Object-Relational Mapper for the **EZ** programming language. It gives you declarative model definitions, automatic table creation, a chainable query builder, and a Unit-of-Work style session for inserts/updates/deletes — backed by a native SQLite driver over FFI.

```
use "orm"
```

---

## Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
- [API Reference](#api-reference)
  - [Column](#column)
  - [Engine](#engine)
  - [Session](#session)
  - [Query](#query)
  - [SQLiteDriver](#sqlitedriver)
  - [Errors](#errors)
- [Defining Relationships](#defining-relationships)
- [Transactions](#transactions)
- [Known Limitations](#known-limitations)
- [FAQ](#faq)

---

## Installation

The ORM is distributed as an `ezlib` package named `orm`. Place it on your `ezlib` path (or alongside your project) and import it as a single package:

```ez
use "orm"
```

This pulls in every public piece of the ORM — models, the query engine, the session, the default SQLite driver, and the error hierarchy — through the package's `main.ez` entry point (as declared in `package.ez`).

**Requirements:**
- `sqlite3.dll` (Windows) or `libsqlite3-0.dll`, discoverable on your system `PATH` or placed next to your project / in a `dlls/` folder.

---

## Quick Start

```ez
use "orm"

# 1. Define a model
model User {
    init() {
        self.id    = Column("integer", primaryKey=true, autoIncrement=true)
        self.name  = Column("string", nullable=false)
        self.email = Column("string", unique=true, nullable=false)
        self.age   = Column("integer", default=18)
    }
}

# 2. Wire up the engine and driver
engine = Engine(SQLiteDriver("app.sqlite3"))
engine.register(User, { "table": "users" })
engine.createAll()

# 3. Use a session to persist data
session = Session(engine)

u = User()
u.name  = "Abdullah"
u.email = "abdullah@ez.org"
session.add(u)
session.commit()

out "New user ID: " + str(u.id)

# 4. Query
adults = session.query(User).filter("age", ">=", 18).orderBy("age", "DESC").all()

session.close()
engine.close()
```

---

## Core Concepts

| Concept   | Role |
|-----------|------|
| **Column**  | Describes a single field's schema: SQL type, constraints, defaults, and foreign keys. |
| **Model**   | A plain EZ `model` whose `init()` assigns `Column(...)` to each mapped property. |
| **Engine**  | Owns the driver connection, holds the schema registry (which models map to which tables), and can create/drop tables. |
| **Session** | A Unit-of-Work: stages `add()`/`delete()` calls and flushes them as INSERT/UPDATE/DELETE statements inside a transaction on `commit()`. |
| **Query**   | A chainable builder for SELECT/DELETE statements, produced via `session.query(ModelClass)`. |
| **Driver**  | The low-level database adapter. Ships with `SQLiteDriver`; any object implementing the same interface can be substituted. |

**Model definition rules:**
- Every model must have `init()` (with no required arguments — the ORM introspects a model by instantiating it with no args).
- Every model must declare exactly one column with `primaryKey=true`.
- A model must have at least one `Column(...)` assignment, or registration will fail.

---

## API Reference

### Column

```ez
Column(type, primaryKey=false, autoIncrement=false, unique=false, nullable=true, default=nil, foreignKey=nil)
```

| Parameter       | Type    | Default | Description |
|-----------------|---------|---------|-------------|
| `type`          | string  | —       | Logical column type. See type mapping below. |
| `primaryKey`    | bool    | `false` | Marks this column as the table's primary key. Exactly one column per model should set this. |
| `autoIncrement` | bool    | `false` | Adds `AUTOINCREMENT`. Only meaningful alongside `primaryKey=true` on an `integer` column. When set, the ORM populates the field automatically after `INSERT`. |
| `unique`        | bool    | `false` | Adds a `UNIQUE` constraint. Ignored if `primaryKey` is also `true` (redundant). |
| `nullable`      | bool    | `true`  | If `false`, adds `NOT NULL`. Ignored if `primaryKey` is also `true`. |
| `default`       | any     | `nil`   | Adds a `DEFAULT` clause. Strings are automatically quoted. |
| `foreignKey`    | string  | `nil`   | References another model's column, in `"ModelName.column"` format (e.g. `"User.id"`). |

**Type mapping** (case-insensitive):

| You write                          | SQL column type |
|-------------------------------------|------------------|
| `"integer"`, `"int"`                | `INTEGER` |
| `"string"`, `"text"`, `"str"`       | `TEXT` |
| `"float"`, `"real"`, `"double"`, `"number"` | `REAL` |
| `"bool"`, `"boolean"`               | `INTEGER` (0/1) |
| `"blob"`, `"bytes"`                 | `BLOB` |
| *(anything else)*                   | `TEXT` (fallback) |

---

### Engine

The Engine owns the driver connection and the schema registry.

```ez
Engine(driver)
```

| Method | Signature | Description |
|--------|-----------|--------------|
| `register` | `register(modelClass, options={})` | Introspects `modelClass`'s `Column` definitions and registers it. `options["table"]` overrides the default table name (which is otherwise the model's class name). Throws `SchemaError` if there are no columns or no primary key. |
| `createAll` | `createAll()` | Runs `CREATE TABLE IF NOT EXISTS` for every registered model, including foreign key constraints. |
| `dropAll` | `dropAll()` | Runs `DROP TABLE IF EXISTS` for every registered model. |
| `getMeta` | `getMeta(modelClass)` | Returns the internal schema metadata dictionary (`table`, `columns`, `pk`, `foreignKeys`) for a registered model. Throws `SchemaError` if the model isn't registered. |
| `close` | `close()` | Closes the underlying driver connection. |

**Registration order matters for foreign keys:** register the *referenced* model before the model that references it, so `createAll()` can resolve the referenced table name correctly.

---

### Session

The Session batches pending changes and flushes them together inside a transaction.

```ez
Session(engine)
```

| Method | Signature | Description |
|--------|-----------|--------------|
| `query` | `query(modelClass)` | Returns a new `Query` for the given model. |
| `add` | `add(instance)` | Stages an instance for INSERT (if its primary key is unset) or UPDATE (if it's already set). Nothing is written until `commit()`. |
| `delete` | `delete(instance)` | Stages an instance for DELETE by its primary key. |
| `begin` | `begin()` | Explicitly starts a transaction on the underlying driver. |
| `commit` | `commit()` | Flushes all staged adds/deletes as INSERT/UPDATE/DELETE statements, wrapped in a transaction. Rolls back and clears staged operations automatically if any statement throws. |
| `rollback` | `rollback()` | Rolls back the current transaction and clears any staged operations. |
| `close` | `close()` | Clears staged operations. Does **not** close the engine/driver — sessions don't own the connection. |

**Insert vs. update detection:** `add()` doesn't immediately decide; the decision happens at `commit()` time, based on whether the instance's primary-key field currently holds a real value or is still an unset `Column` placeholder.

---

### Query

Produced via `session.query(ModelClass)`. All builder methods except the terminal ones return `self`, so calls can be chained.

| Method | Signature | Returns | Description |
|--------|-----------|---------|--------------|
| `filter` | `filter(field, op, value)` or `filter(field, value)` | `Query` | Adds a `WHERE` condition. Two-argument form defaults the operator to `"="`. Supports any SQL comparison operator string (`"="`, `"!="`, `">"`, `">="`, `"<"`, `"<="`, `"LIKE"`, etc.). Multiple `filter()` calls are combined with `AND`. |
| `orderBy` | `orderBy(field, direction="ASC")` | `Query` | Adds an `ORDER BY` clause. Call multiple times for multi-column sorts. |
| `limit` | `limit(n)` | `Query` | Adds a `LIMIT`. |
| `offset` | `offset(n)` | `Query` | Adds an `OFFSET`. |
| `all` | `all()` | array of instances | Executes the query and hydrates matching rows into model instances. |
| `first` | `first()` | instance or `nil` | Executes with an implicit `LIMIT 1` and returns the first match, or `nil`. |
| `count` | `count()` | integer | Executes a `COUNT(*)` with the same filters applied. |
| `delete` | `delete()` | integer | Executes a bulk `DELETE` matching the current filters, and returns the number of affected rows. |

**Example:**

```ez
recent_adults = session.query(User)
    .filter("age", ">=", 18)
    .filter("name", "LIKE", "A%")
    .orderBy("age", "DESC")
    .limit(10)
    .all()
```

---

### SQLiteDriver

The default driver, implemented over SQLite's C API via FFI.

```ez
SQLiteDriver(path)
```

- Automatically enables `PRAGMA journal_mode=WAL` and `PRAGMA foreign_keys=ON` on connect.
- Looks for `sqlite3.dll`, then `libsqlite3-0.dll`, then `dlls\libsqlite3-0.dll`, in that order.

| Method | Description |
|--------|-------------|
| `execute(sql, params)` | Runs a non-SELECT statement with positional `?` parameters. |
| `query(sql, params)` | Runs a SELECT and returns an array of row dictionaries. |
| `lastInsertId()` | Returns the rowid of the last INSERT. |
| `changes()` | Returns the number of rows affected by the last statement. |
| `begin()` / `commit()` / `rollback()` | Transaction control. |
| `close()` | Closes the connection handle. |

You can swap in your own driver for a different backend by implementing this same method set — `Engine` and `Session` only depend on this interface, not on SQLite specifically.

---

### Errors

All ORM exceptions extend the built-in `Exception` model, via a common `ORMError` base.

| Error | Thrown when |
|-------|-------------|
| `ORMError` | Base class for all ORM-specific errors. |
| `SchemaError` | A model is registered without columns or without a primary key, or a query references an unregistered model. |
| `IntegrityError` | An INSERT is missing a required (non-nullable, no-default) value. |
| `DriverError` | A driver-level failure: DB open/prepare/step failure, or a missing SQLite library. |

```ez
try {
    engine.register(BrokenModel)
} catch e {
    out e.toString()   # e.g. "SchemaError: Model 'BrokenModel' has no primary key..."
}
```

---

## Defining Relationships

Foreign keys are declared on the referencing column and resolved by table name at `createAll()` time:

```ez
model Post {
    init() {
        self.id      = Column("integer", primaryKey=true, autoIncrement=true)
        self.title   = Column("string", nullable=false)
        self.user_id = Column("integer", foreignKey="User.id")
    }
}
```

`"User.id"` refers to the **model name** and its **column**, not the physical table name — the ORM looks up `User`'s registered table internally when generating the `FOREIGN KEY` clause. Register the referenced model (`User`) before the referencing model (`Post`) if you want the generated SQL to use the correct table name.

The ORM does not currently provide relationship-loading helpers (no `.related()`/lazy-loading) — foreign keys are enforced at the database level only. You query related rows explicitly via `session.query(...)`.

---

## Transactions

```ez
session.begin()
# ... session.add(...) / session.delete(...) ...
session.commit()
```

`commit()` always flushes staged adds/deletes inside a transaction and clears them afterward; on any error it rolls back automatically and clears pending state before re-throwing. Prefer letting `commit()` manage the transaction implicitly (just call `add()`/`delete()` then `commit()`) unless you specifically need to group operations that aren't staged through the session (e.g. mixing raw driver calls with session operations).

---

## Known Limitations

- **Filtering on `nil`:** `.filter(field, nil)` currently compiles to `field = ?` with a bound NULL parameter. Per SQL semantics, `x = NULL` never evaluates true — so this does **not** match NULL rows the way you'd expect from `IS NULL`. If you need to query for NULLs, do so with a raw driver call until this is addressed.
- **`begin()` + `commit()` together:** calling `session.begin()` explicitly and then `session.commit()` will attempt to start a second transaction on top of the first, which SQLite rejects. Use one or the other for a given unit of work — either call `begin()`/`commit()` around raw operations, or just call `add()`/`delete()` followed by `commit()` and let it manage the transaction itself.
- **`ORDER BY` / `LIMIT` / `OFFSET` on bulk `delete()`:** chaining `.orderBy()`, `.limit()`, or `.offset()` before `.delete()` produces SQL that plain SQLite doesn't accept on `DELETE` statements. Use `.filter()` alone to scope bulk deletes.
- **No relationship loading:** foreign keys are schema-level only; there's no `.join()` or eager/lazy relationship loading yet.
- **No migrations:** `createAll()`/`dropAll()` are the only schema operations — there's no incremental migration support for evolving an existing table's columns.

---

## FAQ

**Do I need to call `engine.register()` before using a model?**
Yes — the registry is what lets `Session`/`Query` know a model's table name, columns, and primary key. Register every model right after constructing the `Engine`, before `createAll()`, `session.query()`, or `session.add()`.

**Can I use multiple databases at once?**
Yes — create separate `Engine(SQLiteDriver(...))` instances, one per database file, and register the relevant models against each.

**How do I inspect the generated SQL for a table?**
`engine.getMeta(ModelClass)["columns"]` gives you each column's computed `sqlDef` string, and `Column.toSQL()` can be called directly on a standalone `Column` instance.
