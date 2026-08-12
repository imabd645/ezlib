# postgres

PostgreSQL for EZ, over libpq. Parameterised queries, connection pooling,
transactions with savepoints and retry, full type mapping, and errors you can
branch on.

```ez
use "postgres"

db = connect("postgres://user:pass@localhost/appdb")

rows = db.query("select * from users where age > $1", [18])
user = db.queryOne("select * from users where email = $1", [email])
db.execute("update users set seen_at = now() where id = $1", [id])

db.transaction(|tx| {
    tx.execute("insert into orders (user_id) values ($1)", [id])
    tx.execute("update stock set n = n - 1 where id = $1", [item])
})
```

## Install

```
ez install postgres
```

No EZ dependencies. Needs `libpq.dll`, which comes with PostgreSQL — see
[Getting libpq](#getting-libpq).

## No SQL is ever built by concatenation

There is no API here that interpolates a value into a statement. Values travel
beside the statement as parameters, so a value containing a quote or a semicolon
is *data* and can never become syntax.

```ez
db.execute("insert into people (name) values ($1)", ["Robert'); drop table people;--"])
```

That stores the string. The table is fine. The test suite asserts exactly this.

Table and column names **cannot** be parameterised — SQL does not allow it — so
the helpers that take them refuse anything that is not a plain identifier rather
than trying to escape it. Escaping there is a judgement call, and this is not a
place for judgement calls.

## Querying

```ez
db.query(sql, params)          # every row, as dictionaries
db.queryOne(sql, params)       # the first row, or nil
db.queryValue(sql, params)     # a single value: count(*), a returned id
db.queryColumn(sql, params)    # one column across every row
db.execute(sql, params)        # affected row count
db.exists(sql, params)         # wraps it in select exists(…)
db.queryResult(sql, params)    # the full Result
```

`Result` carries `rows`, `columns` (with names, OIDs and type names),
`affected`, and the helpers `count`, `isEmpty`, `first`, `last`, `column(name)`,
`scalar`, `columnNames`.

### Writing without writing SQL

```ez
row = db.insert("people", { "name": "Ada", "email": "ada@x.test" }, "id")
db.update("people", { "age": 42 }, "name = $1", ["Ada"])
db.delete("people", "id = $1", [id])
db.insertMany("people", [{ "name": "a" }, { "name": "b" }])
```

`update` renumbers your `$1` past the placeholders it generated for the SET
list, so the two sets never collide.

`insertMany` sends one INSERT with many VALUES rows. The round trip is what
costs, so this is dramatically faster than a loop for anything above a handful
of rows.

## Types

| PostgreSQL | EZ |
|---|---|
| `int2` `int4` `int8` | number |
| `float4` `float8` | number |
| `numeric` | **string** — see below |
| `bool` | bool |
| `text` `varchar` `uuid` | string |
| `json` `jsonb` | parsed dictionary/list |
| `bytea` | list of byte values |
| `text[]` `int[]` … | list |
| `NULL` | `nil` |
| timestamps, dates | string, as the server formatted them |

**`numeric` stays a string on purpose.** It is the type people choose for money
*because* it is exact, and turning it into a float here would throw that away
silently. Pass it to `decimal`, or call `num()` if you genuinely want a float.

**`NULL` is not an empty string.** libpq reports nullness separately from the
value, and that is checked before the text is looked at. A `nil` parameter is
sent as a null pointer, not as `""`.

**Arrays** are parsed properly, not split on commas — Postgres quotes any
element containing a comma, brace, quote or backslash, and an *unquoted* `NULL`
is the SQL null while the quoted string `"NULL"` is four characters of text.

**`bytea`** round-trips byte-exactly through the hex text format, so there is no
encoding step in the path to corrupt an image.

```ez
db.execute("insert into files (data) values ($1)", [encodeBytea(bytes)])
bytes = db.queryValue("select data from files where id = $1", [id])
```

## Transactions

```ez
db.transaction(|tx| {
    tx.execute(…)
    tx.execute(…)
})
```

Any throw rolls back and re-raises, so a half-finished transaction can never be
committed by accident — and the original error is what surfaces, not a rollback
error.

**Nested calls become savepoints**, because a second `BEGIN` is an error in
Postgres. An inner failure rolls back only the inner work.

```ez
db.transactionWith("isolation level serializable", body)
db.transactionWith("read only", body)
db.begin()   db.commit()   db.rollback()      # manual, when a callback does not fit
db.isAborted()                                # Postgres refuses everything until rollback
```

### Retrying

```ez
db.withRetry(3, |tx| { … })
```

Only **serialization failures (40001)** and **deadlocks (40P01)** are retried.
Those mean "the database is fine, you lost a race", and the whole transaction
has to run again from the start. Retrying a unique violation just produces four
of them, so it does not.

The delay grows a little each attempt, so two transactions retrying into each
other do not simply collide again.

## Errors

Every failure carries the SQLSTATE on the exception, so you can branch on it
without matching on message text — which is localised and changes between
versions.

```ez
try { db.insert("users", row) }
catch (e) {
    when isUniqueViolation(e.sqlstate) { give "that email is taken" }
    throw e
}
```

Also on the exception: `constraint`, `table`, `column`, `detail`.

The message says what to do about it, and carries the statement — a database
error without the SQL is nearly useless when it surfaces in a log an hour later:

```
postgres: duplicate key value violates unique constraint "people_email_key"
(the 'people_email_key' constraint -- this row already exists) [23505]
  detail: Key (email)=(ada@x.test) already exists.
  sql: insert into "people" ("name", "email") values ($1, $2)
```

Classifiers: `isUniqueViolation`, `isForeignKeyViolation`, `isNotNullViolation`,
`isCheckViolation`, `isConstraintViolation`, `isRetryable`,
`isConnectionError`, `errorClass`, `describeClass`.

## Pooling

Opening a connection to Postgres is expensive: a TCP handshake, TLS,
authentication, then a backend process fork on the server. A request that opens
its own spends more time connecting than querying.

```ez
db = pool("postgres://…", 10)

db.query(sql, params)                  # borrow, run, return
db.transaction(body)
db.use(|c| { … })                      # several statements on one connection
db.stats()                             # { size, idle, leased, opened }
```

`use` returns the connection **even when the body throws** — the case people
forget, and the reason a pool leaks one connection per failure until it is
exhausted. A connection handed back mid-transaction is rolled back first, and
one that died while idle is discarded rather than handed out.

> `SET` is per-session, so a pooled connection does not inherit a
> `set search_path` or `set timezone` you ran on another one. Put such settings
> in the connection string: `?options=-c search_path%3Dmyschema`.

## Connecting

```ez
connect("postgres://user:pass@host:5432/db?sslmode=require")
connect("host=localhost user=me dbname=app")          # libpq keyword form
connect({ "host": "localhost", "dbname": "app" })
connect(fromEnv())                                    # DATABASE_URL, or PGHOST/PGUSER/…
```

Values are quoted and escaped when building the keyword string, because a
password containing a space or a quote would otherwise truncate the connection
string silently.

```ez
db.withStatementTimeout(5000)           # cancel anything slower
db.onQuery(|sql, params, ms| log(sql, ms))
db.ping()          db.isOpen()          db.ensureOpen()      # reconnect if needed
db.serverVersionText()
db.tables()        db.tableExists(name)  db.columnsOf(name)
```

`ping` does a round trip rather than checking the socket: a connection can look
fine and be unusable after the server restarts.

## Getting libpq

`libpq.dll` ships with PostgreSQL, not with Windows. The package looks for it
on `PATH`, in `C:/Program Files/PostgreSQL/<version>/bin`, in the MSYS2 mingw64
bin directory, and in a `dlls/` folder beside your script. Failing that:

```ez
useLibrary("C:/path/to/libpq.dll")     # before connecting
```

Installing the PostgreSQL **client** alone is enough — you do not need a local
server. `libraryPath()` reports which copy was loaded, and the error when none
is found lists everywhere it looked.

## Tests

```
ez test.ez
```

156 assertions against a real server. The parts that need no server — URL
parsing, encoding, array parsing, SQLSTATE classification — run either way; the
live section is skipped with a count if nothing is listening.

```
docker run -d --name ez-pg -e POSTGRES_PASSWORD=ezpass -e POSTGRES_USER=ezuser \
           -e POSTGRES_DB=ezdb -p 55432:5432 postgres:16-alpine
```

Everything is created and dropped in a schema of its own, so the suite cannot
touch anything already in the database.

## License

MIT
