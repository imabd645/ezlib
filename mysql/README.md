# mysql

MySQL and MariaDB for EZ, over pure TCP.

```ez
use "mysql"

db = connect({ "host": "localhost", "user": "app", "password": "…",
               "database": "shop" })

db.execute("INSERT INTO users (name) VALUES (?)", ["ana"])
rows = db.query("SELECT * FROM users WHERE name = ?", ["ana"])
db.close()
```

## Install

```
ez install mysql
```

Depends on `socket` and `crypto`. **No client library to install** — this
speaks the MySQL wire protocol directly, the same way `redis` and `websocket`
do. `libmysql.dll` / `libmariadb.dll` are not needed and do not have to be
found on the path.

## Parameters are bound, not escaped

Through real prepared statements — `COM_STMT_PREPARE` and `COM_STMT_EXECUTE`.

Escaping is a weaker guarantee: it depends on getting every type, charset and
edge case right, and it is the mechanism behind most injection that survives a
"we escape our inputs" review. A bound value travels beside the statement and
is never parsed as SQL:

```ez
db.query("SELECT * FROM users WHERE name = ?", ["O'Brien\"; DROP TABLE users; --"])
```

That is a search for a customer with an odd name, and nothing else.

Identifiers cannot be bound by any database, so `quoteIdentifier` is provided
and doubles an embedded backtick, MySQL's own rule.

## Querying

```ez
db.query(sql, params)        # rows as dictionaries
db.queryOne(sql, params)     # the first row, or nil
db.queryValue(sql, params)   # the first column of the first row
db.execute(sql, params)      # affected rows
db.exists(sql, params)       # is there at least one row
db.run(sql, params)          # a Result with rows, affected, insertId, columns
```

Without parameters the statement is sent as text, which is what DDL and utility
statements need.

```ez
db.insert("users", { "name": "ana", "email": "a@b.c" })   # returns the new id
db.tables()      db.tableExists("users")
```

## Transactions

```ez
db.transaction(|c| {
    c.execute("UPDATE accounts SET balance = balance - ? WHERE id = ?", [100, 1])
    c.execute("UPDATE accounts SET balance = balance + ? WHERE id = ?", [100, 2])
})
```

Rolls back if the block throws. `begin()`, `commit()` and `rollback()` are
there for manual control.

## Errors

The MySQL error code and SQLSTATE are attached, so you can branch on them
rather than matching localised message text:

```ez
try { db.insert("users", user) }
catch (e) {
    when isDuplicateKey(e) { give "that email is already registered" }
    when isDeadlock(e) { give retry() }
    throw e
}
```

`isDuplicateKey` `isDeadlock` `isLockTimeout` `isMissingTable` `isAccessDenied`

A failed statement leaves the connection usable.

## Types

| MySQL | EZ |
|---|---|
| TINY, SHORT, LONG, LONGLONG, YEAR | integer, sign preserved |
| FLOAT, DOUBLE | float |
| DECIMAL, NEWDECIMAL | **string** |
| VARCHAR, TEXT, BLOB, JSON, ENUM | string |
| DATE, DATETIME, TIMESTAMP | ISO-ish string |
| TIME | `HH:MM:SS` string |
| NULL | `nil`, distinct from `""` |

DECIMAL stays text deliberately. It is exact, and converting it would put it
through a float and lose the exactness that was the reason to choose it.

## Authentication

`mysql_native_password` — the MariaDB default and available on every MySQL —
works out of the box. Neither scheme puts the password on the wire.

**MySQL 8 defaults to `caching_sha2_password`.** Its fast path is supported,
but completing a cold-cache authentication over a plaintext connection requires
encrypting the password with the server's RSA key, which this client does not
implement. It says so rather than hanging:

```sql
ALTER USER 'app'@'%' IDENTIFIED WITH mysql_native_password BY '…';
```

## Connection strings

```ez
db = fromUrl("mysql://user:password@host:3306/database")
db.open()
```

## Tests

```
ez test.ez
```

93 assertions against a real server, because a wire protocol tested against a
mock is tested against your own reading of the specification:

```
docker run -d -p 33061:3306 -e MARIADB_ROOT_PASSWORD=ezpass \
  -e MARIADB_DATABASE=ezdb -e MARIADB_USER=ezuser \
  -e MARIADB_PASSWORD=ezpass mariadb:11
```

Without one the live section is skipped and the skip is reported.

## License

MIT
