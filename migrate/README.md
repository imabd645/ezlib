# migrate — schema migrations for EZ

> **Import:** `use "migrate"`
> **Install:** `ez install migrate`
> **Depends on:** `orm`, `fs`

```ez
use "orm"
use "migrate"

engine = Engine(SQLiteDriver("app.db"))
m = Migrator(engine.driver)

m.add("001_users",
    |db| db.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)", []),
    |db| db.execute("DROP TABLE users", []))

m.up()
```

---

## Why you need this

The ORM's own README says it plainly:

> `createAll()` uses `CREATE TABLE IF NOT EXISTS`, so changing a model does
> **not** alter an existing table — the schema silently drifts.

Add a column to a model, deploy, and the live table is unchanged. Nothing
errors until a query touches the missing column, usually in production. A
migration is the record of *how* the schema got to where it is, applied in
order, exactly once each.

## Declaring

```ez
m.add(id, up, down)
```

`id` both orders the migration and identifies it forever, so use a sortable
prefix — `001_users`, or `20260812_1430_add_email` if several people write
them. Migrations run in **id order**, not declaration order.

`up` and `down` each receive the driver. `down` may be `nil` for something
that cannot be undone; rolling back past it then fails loudly rather than
pretending it worked.

## Running

| Call | Effect |
| --- | --- |
| `m.up()` | Apply everything pending, oldest first. Returns the ids applied |
| `m.upTo(id)` | Apply up to and including `id` |
| `m.down(n)` | Roll back the last `n`, newest first. Default 1 |
| `m.reset()` | Roll everything back |
| `m.redo()` | Roll back one and reapply it |

**`up()` is idempotent** — running it twice applies nothing the second time.
That is what makes it safe to run on every deploy, which is where it belongs.

## Failure leaves nothing behind

Each migration runs in a transaction, and the ledger row is written in the
*same* transaction. If a migration fails half way:

- the schema is unchanged — no half-created tables
- it is **not** recorded as applied, so the next run retries it
- earlier migrations in the same batch stay applied

Recording a migration that did not fully apply is worse than not recording one
that did, which is why the two are committed together.

```ez
try { m.up() }
catch (e) { out e.message }
# migrate: '002_add_email' failed going up -- near "ALTER": syntax error
```

## Inspecting

| Call | Result |
| --- | --- |
| `m.pending()` | Ids not yet applied |
| `m.applied()` | Ids recorded in the database |
| `m.status()` | `[{ id, applied, changed }]` |
| `m.orphans()` | Applied but no longer declared |
| `m.drifted()` | Applied, but the source has changed since |
| `m.report()` | A readable summary |

```
  applied  001_users
  applied  002_email
  pending  003_index
```

**Orphans** mean the database is ahead of the code — a deleted file, or a
deploy rolled back without rolling back its migrations. Worth checking before
shipping.

## Detecting an edited migration

The most damaging mistake is editing a migration that has already run: it
applies cleanly on a fresh database and does nothing on an existing one, so
two environments diverge with no error anywhere.

Pass the source text and that becomes detectable:

```ez
sql = "CREATE TABLE users (id INTEGER PRIMARY KEY)"
m.addWithSource("001_users", |db| db.execute(sql, []), nil, sql)

m.drifted()      # ["001_users"] once the text changes
```

`report()` marks those as `CHANGED`. The fingerprint is a cheap content hash,
not a cryptographic one — it only has to notice that the text changed.

## Loading migrations from a directory

`m.add(...)` keeps everything in code, in one file or a registry module that
imports one file per migration. If you'd rather keep each migration as its
own `.sql` file, `addDir` reads a whole folder:

```ez
m.addDir("migrations")
```

One migration per id, two files:

```
migrations/
  001_users.up.sql
  001_users.down.sql
  002_add_email.up.sql
  002_add_email.down.sql
```

`.down.sql` is optional — a migration with no down file can't be rolled back,
same as passing `nil` to `add()` directly. For something with no rollback at
all, skip the pair and write a single file instead:

```
migrations/
  003_backfill.sql
```

Anything else in the folder — a `README.md`, a `.gitkeep` — is ignored rather
than rejected, so the migrations directory can carry its own documentation.

A file may hold more than one statement, separated by `;`:

```sql
-- 001_users.up.sql
CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT);
CREATE INDEX idx_users_name ON users (name);
```

sqlite (and the other drivers here) only prepare one statement at a time, so
`addDir` splits each file before running it. The splitter understands `'...'`
and `"..."` string literals and `--` / `/* */` comments well enough not to
split inside them, but it is not a SQL parser — a semicolon inside something
stranger, such as a Postgres dollar-quoted function body, will still split
wrongly. Keep those as one statement per file.

Everything declared through `addDir` behaves exactly like a migration added
by hand: it runs in id order, in a transaction, with a checksum taken from the
file's own text — so `drifted()` catches an edited `.sql` file the same way
it catches an edited closure.

## A migration CLI

Pairs with `args`, which is usually how this gets run:

```ez
use "args"
use "orm"
use "migrate"

cli = Args("migrate", "Apply database migrations")
cli.positional("command", "up, down, status").withDefault("status")
cli.option("steps", "n", "How many to roll back").number().withDefault(1)
opts = cli.parse(argv)

m = Migrator(Engine(SQLiteDriver("app.db")).driver)
# ... m.add(...) for each migration ...

when opts["command"] == "up"     { m.up() }
other when opts["command"] == "down" { m.down(opts["steps"]) }
other                            { out m.report() }
```

## Notes

- **Migrations can be declared in code or loaded from a directory** —
  `m.add(...)` for the former, `m.addDir("migrations")` for the latter. Mix
  both if you want; ids just have to stay unique across the two.
- **The ledger lives in `schema_migrations`** alongside your data, so it
  travels with the database rather than with the code.
- **`down` is for development.** Reversing a migration that dropped a column
  cannot bring the data back. In production, roll forward with a new
  migration.
- Works with any ORM driver — SQLite, Postgres, MySQL — since it only uses
  `execute`, `query` and the transaction methods.

## Testing

```
ez test.ez
```

72 tests covering ordering, idempotence, rollback, targeted application,
irreversible migrations, drift and orphan detection, the ledger, directory
loading (naming convention, statement splitting, missing files, ignored
files), and — most importantly — that a failed migration leaves neither
schema changes nor a ledger row behind.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Migrator`: declaring, running, inspecting, reporting |
| `store.ez` | The `schema_migrations` ledger and content fingerprinting |
| `files.ez` | Reading `.sql` files from a directory and splitting them into statements |

## License

MIT
