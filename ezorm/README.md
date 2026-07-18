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
Column(type, primaryKey=false, autoIncrement=false, unique=false, nullable=true, default=nil, foreignKey=nil, index=false)
```

| Parameter       | Type    | Default | Description |
|-----------------|---------|---------|-------------|
| `type`          | string  | —       | Logical column type. See type mapping below. An unrecognised type raises `SchemaError`. |
| `primaryKey`    | bool    | `false` | Marks this column as the table's primary key. Exactly one column per model may set this. |
| `autoIncrement` | bool    | `false` | Adds `AUTOINCREMENT`. Only meaningful alongside `primaryKey=true` on an `integer` column. When set, the ORM populates the field automatically after `INSERT`. |
| `unique`        | bool    | `false` | Adds a `UNIQUE` constraint. Ignored if `primaryKey` is also `true` (redundant). |
| `nullable`      | bool    | `true`  | If `false`, adds `NOT NULL`. Ignored if `primaryKey` is also `true`. |
| `default`       | any     | `nil`   | Adds a `DEFAULT` clause. String defaults are quoted and any embedded apostrophe is escaped; booleans render per dialect. |
| `foreignKey`    | string  | `nil`   | References another model's column, in `"ModelName.column"` format (e.g. `"User.id"`). A malformed reference raises `SchemaError` instead of being silently dropped. |
| `index`         | bool    | `false` | Creates a secondary index on this column during `createAll()`. |

**Type mapping** (case-insensitive):

| You write                          | SQLite | MySQL | PostgreSQL |
|-------------------------------------|--------|-------|------------|
| `"integer"`, `"int"`                | `INTEGER` | `INT` | `INTEGER` |
| `"bigint"`, `"long"`                | `INTEGER` | `BIGINT` | `BIGINT` |
| `"string"`, `"text"`, `"str"`       | `TEXT` | `VARCHAR(255)` | `TEXT` |
| `"float"`, `"real"`, `"double"`, `"number"` | `REAL` | `DOUBLE` | `DOUBLE PRECISION` |
| `"bool"`, `"boolean"`               | `INTEGER` (0/1) | `TINYINT(1)` | `BOOLEAN` |
| `"blob"`, `"bytes"`                 | `BLOB` | `BLOB` | `BYTEA` |
| `"date"`                            | `TEXT` | `DATE` | `DATE` |
| `"datetime"`, `"timestamp"`         | `TEXT` | `DATETIME` | `TIMESTAMP` |
| *(anything else)*                   | raises `SchemaError` | | |

Values are coerced back to the declared type on read, so a `bool` column reads as `true`/`false` rather than SQLite's `0`/`1` or MySQL's `"0"`/`"1"`.

---

### Engine

The Engine owns the driver connection and the schema registry.

```ez
Engine(driver)
```

| Method | Signature | Description |
|--------|-----------|--------------|
| `register` | `register(modelClass, options={})` | Introspects `modelClass`'s `Column` definitions and registers it. `options["table"]` overrides the default table name (otherwise the class name). Throws `SchemaError` if there are no columns, no primary key, more than one primary key, or if a different model is already registered under the same name. Re-registering the same class is a no-op. |
| `createAll` | `createAll()` | Runs `CREATE TABLE IF NOT EXISTS` for every registered model, plus any requested indexes. Tables are created in foreign-key dependency order. |
| `dropAll` | `dropAll()` | Runs `DROP TABLE IF EXISTS` for every registered model, in reverse dependency order (children before parents). |
| `getMeta` | `getMeta(modelClass)` | Returns the internal schema metadata dictionary (`table`, `columns`, `pk`, `foreignKeys`, `indexes`) for a registered model. Throws `SchemaError` if the model isn't registered. |
| `close` | `close()` | Closes the underlying driver connection. |

`Engine` exposes the resolved SQL dialect as `engine.dialect`, read from the driver's required `dialect` property.

**Registration order for foreign keys:** register the *referenced* model before the model that references it — `createAll()` raises `SchemaError` if a foreign key names an unregistered model. Creation *order* is then sorted automatically by dependency, so you don't need to worry about which table gets created first.

---

### Session

The Session batches pending changes and flushes them together inside a transaction.

```ez
Session(engine)
```

| Method | Signature | Description |
|--------|-----------|--------------|
| `query` | `query(modelClass)` | Returns a new `Query` for the given model. |
| `get` | `get(modelClass, pkValue)` | Fetches a single instance by primary key, or `nil`. |
| `add` | `add(instance)` | Stages an instance for INSERT (if its primary key is unset) or UPDATE (if it's already set). Staging the same instance twice is a no-op. Nothing is written until `commit()`. |
| `addAll` | `addAll(instances)` | Stages every instance in an array. |
| `delete` | `delete(instance)` | Stages an instance for DELETE by its primary key. Throws `IntegrityError` if the instance has no primary key value. |
| `begin` | `begin()` | Explicitly starts a transaction. Nesting is tracked, so `begin()` followed by `commit()` works correctly. |
| `commit` | `commit()` | Flushes all staged adds/deletes, wrapped in a transaction (opening one only if none is already open). Rolls back and clears staged operations automatically if any statement throws. |
| `rollback` | `rollback()` | Rolls back the current transaction and clears any staged operations. |
| `close` | `close()` | Rolls back any open transaction and clears staged operations. Does **not** close the engine/driver — sessions don't own the connection. |

**Batching:** at `commit()`, pending inserts are grouped by model and by which columns are set, and rows sharing a column set go out as a single multi-row `INSERT` — rather than one round trip per object. Rows whose primary key is database-generated are inserted individually so each generated id can be read back.

**Insert vs. update detection:** `add()` doesn't immediately decide; the decision happens at `commit()` time, based on whether the instance's primary-key field currently holds a real value or is still an unset `Column` placeholder.

---

### Query

Produced via `session.query(ModelClass)`. All builder methods except the terminal ones return `self`, so calls can be chained.

| Method | Signature | Returns | Description |
|--------|-----------|---------|--------------|
| `filter` | `filter(field, op, value)` or `filter(field, value)` | `Query` | Adds a `WHERE` condition. Two-argument form defaults the operator to `"="`. Multiple `filter()` calls are combined with `AND`. `field` must name a column the model declares, and `op` must be one of `=`, `!=`, `<>`, `<`, `<=`, `>`, `>=`, `LIKE`, `NOT LIKE`, `IN`, `NOT IN`, `IS`, `IS NOT` — anything else raises `SchemaError`. `IN`/`NOT IN` take an array and expand to one bound placeholder per element. |
| `filterNull` | `filterNull(field, isNull=true)` | `Query` | Adds `field IS NULL` (or `IS NOT NULL`). |
| `orderBy` | `orderBy(field, direction="ASC")` | `Query` | Adds an `ORDER BY` clause. Call multiple times for multi-column sorts. `direction` must be `ASC` or `DESC`. |
| `limit` | `limit(n)` | `Query` | Adds a `LIMIT`. |
| `offset` | `offset(n)` | `Query` | Adds an `OFFSET`. |
| `clone` | `clone()` | `Query` | Returns an independent copy, so a partially built query can be reused as a base for several queries. |
| `all` | `all()` | array of instances | Executes the query and hydrates matching rows into model instances, coercing each value to its declared column type. |
| `first` | `first()` | instance or `nil` | Returns the first match, or `nil`. Runs against a clone, so it does **not** leave `LIMIT 1` attached to the query. |
| `exists` | `exists()` | boolean | True when at least one row matches. |
| `count` | `count()` | integer | Executes a `COUNT(*)` with the same filters. `ORDER BY`/`LIMIT`/`OFFSET` are deliberately not applied. |
| `update` | `update(values)` | integer | Bulk `UPDATE` of the given `{column: value}` dict for matching rows. Requires at least one filter. |
| `delete` | `delete()` | integer | Bulk `DELETE` matching the current filters; returns rows affected. **Requires at least one filter** — an unfiltered call raises `IntegrityError`. |
| `deleteAll` | `deleteAll()` | integer | Deletes every row in the table. The explicit form of an unfiltered delete. |

> **Identifier safety.** Field names, operators and sort directions are validated against the model's schema and a fixed keyword list rather than being concatenated into the SQL. Passing a `sort` value straight from an HTTP query string is therefore safe: an unknown column raises `SchemaError` instead of injecting SQL. Values are always sent as bound parameters.

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
SQLiteDriver(path, busyTimeoutMs=5000, cacheSize=64)
```

- Automatically enables `PRAGMA journal_mode=WAL` and `PRAGMA foreign_keys=ON` on connect.
- Looks for `sqlite3.dll`, then `libsqlite3-0.dll`, then `dlls\libsqlite3-0.dll`, in that order.
- Sets `sqlite3_busy_timeout` so a concurrent writer waits instead of failing immediately with `SQLITE_BUSY`.
- Keeps an LRU cache of up to `cacheSize` prepared statements. Because the ORM issues the same handful of statements repeatedly, this avoids re-parsing and re-planning each one. Pass `cacheSize=0` to disable.
- Uses real parameter binding (`sqlite3_bind_*`), not string interpolation.
- Rejects multi-statement SQL: `prepare_v2` would compile only the first statement and silently skip the rest, so passing `"SELECT 1; SELECT 2"` raises `DriverError` instead.

Statements are finalized in a `finally`, so a mid-iteration error cannot leak a statement handle (which under WAL would otherwise hold a read transaction open and block checkpointing).

| Method | Description |
|--------|-------------|
| `execute(sql, params)` | Runs a non-SELECT statement with positional `?` parameters. |
| `query(sql, params)` | Runs a SELECT and returns an array of row dictionaries. |
| `lastInsertId()` | Returns the rowid of the last INSERT. |
| `changes()` | Returns the number of rows affected by the last statement. |
| `begin()` / `commit()` / `rollback()` | Transaction control. |
| `close()` | Closes the connection handle. |

You can swap in your own driver for a different backend by implementing this same method set — `Engine` and `Session` only depend on this interface, not on SQLite specifically. A custom driver **must** also expose a `dialect` property (`"sqlite"`, `"mysql"`, or `"postgres"`); `Engine` raises `DriverError` without it.

---

### MySQLDriver / PostgresDriver

```ez
MySQLDriver(host, user, password, dbname, port=3306, charset="utf8mb4")
PostgresDriver(connectionString)
```

`MySQLDriver` loads `libmysql.dll` or `libmariadb.dll`; `PostgresDriver` loads `libpq.dll`.

Both accept the same `?` placeholder syntax as the SQLite driver. Internally they escape values with the vendor's own routine (`mysql_real_escape_string`, `PQescapeLiteral`) and splice them in, locating placeholders with a SQL-aware scanner that ignores any `?` inside string literals, quoted identifiers, and comments. A mismatch between placeholder count and supplied parameters raises `DriverError`.

`MySQLDriver` pins the connection charset (default `utf8mb4`) before running any statement, because `mysql_real_escape_string` escapes according to the connection's charset and is not safe on an unknown one. It refuses to connect if the charset cannot be set.

---

## Testing

```
ez orm/tests/run_tests.ez
```

The suite covers the SQL helpers and DDL generation with no database required, then runs an end-to-end SQLite section against an in-memory database (skipped automatically if `sqlite3.dll` is unavailable). It includes regression tests for identifier injection, placeholder scanning, transaction nesting, `count()`/`first()` isolation, batch inserts, and reserved-word column names.

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

- **Models must be constructible with no arguments.** The engine builds a throwaway instance to read your `Column` definitions, so every parameter of a model's `init()` must be optional. A model written as `init(name, age)` cannot be registered — assign those fields after construction instead. Registering such a model raises a `SchemaError` explaining this rather than failing obscurely.
- **No relationship loading:** foreign keys are schema-level only; there's no `.join()` or eager/lazy relationship loading yet.
- **No identity map or dirty tracking:** mutating a fetched instance does nothing until you `session.add()` it again, and fetching the same row twice yields two independent instances.
- **No migrations:** `createAll()`/`dropAll()` are the only schema operations. `createAll()` uses `CREATE TABLE IF NOT EXISTS`, so changing a model does **not** alter an existing table — the schema silently drifts. Drop and recreate, or manage migrations yourself.
- **No composite primary keys:** exactly one `Column` per model may set `primaryKey=true`.
- **MySQL/PostgreSQL bind by escaped interpolation**, not server-side prepared statements. Values go through the vendor's own escaper (`mysql_real_escape_string` on a charset-pinned connection, `PQescapeLiteral`) and placeholders are located with a SQL-aware scanner, so this is safe — but native `mysql_stmt_*` / `PQexecParams` binding would be better still. **SQLite uses real parameter binding.**
- **MySQL/PostgreSQL drivers are less exercised than SQLite.** The test suite covers SQLite end to end; the other two are covered only by the shared SQL-helper tests.

---

## FAQ

**Do I need to call `engine.register()` before using a model?**
Yes — the registry is what lets `Session`/`Query` know a model's table name, columns, and primary key. Register every model right after constructing the `Engine`, before `createAll()`, `session.query()`, or `session.add()`.

**Can I use multiple databases at once?**
Yes — create separate `Engine(SQLiteDriver(...))` instances, one per database file, and register the relevant models against each.

**How do I inspect the generated SQL for a table?**
`engine.getMeta(ModelClass)["columns"]` gives you each column's computed `sqlDef` string, and `Column.toSQL()` can be called directly on a standalone `Column` instance.
