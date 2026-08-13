# mongodb

MongoDB for EZ, over pure TCP.

```ez
use "mongodb"

db = connect({ "host": "localhost", "user": "app", "password": "…",
               "database": "shop" })

users = db.collection("users")
users.insert({ "name": "ana", "age": 31 })
found = users.find({ "age": { "$gt": 30 } })
db.close()
```

## Install

```
ez install mongodb
```

Depends on `socket` and `crypto`. **No driver to install** — this speaks the
wire protocol directly: OP_MSG framing, BSON documents and SCRAM-SHA-256
authentication, the same way `redis` and `mysql` do.

## Collections

```ez
c = db.collection("users")

c.insert(document)              c.insertMany(documents)
c.find(filter, options)         c.findOne(filter, options)
c.findById(id)                  c.count(filter)      c.exists(filter)
c.set(filter, fields)           c.update(filter, changes, options)
c.updateMany(filter, changes)   c.upsert(filter, changes)
c.remove(filter)                c.removeMany(filter)
c.distinct(field, filter)       c.aggregate(pipeline)
c.createIndex(fields, options)  c.indexes()          c.drop()
```

`options` for `find` takes `limit`, `skip`, `sort` and `projection`.

`set(filter, fields)` applies `$set` for you. That matters: an update *without*
an operator **replaces the whole document**, which is the classic way to lose
every field you did not mention.

## Cursors are followed

MongoDB returns the first 101 documents and a cursor id; the rest only arrives
if you ask for it. `find` follows the cursor to the end. Without that a query
over a large collection silently returns a fraction of it and looks like it
worked — the failure mode this package exists to avoid.

## Types

Integers, floats, booleans, strings, `nil`, arrays and nested documents all
round-trip. Integers are stored as int32 where they fit and int64 where they do
not, with sign preserved.

`ObjectId` is its own type, not a string — turned into one it would stop
matching the `_id` it came from:

```ez
user["_id"].hex()                       // "507f1f77bcf86cd799439011"
users.findById(user["_id"])
users.findById("507f1f77bcf86cd799439011")   // hex works too
```

An upsert that inserts hands back the new id, which is the one thing you cannot
work out afterwards:

```ez
r = users.upsert({ "email": e }, { "$set": { "name": n } })
r["matched"]  r["modified"]  r["upserted"]  r["upsertedId"]
```

## Injection

Queries are **documents, not strings**, so there is no query language to inject
into — the structural reason NoSQL injection looks different from SQL injection.

The trap that remains is passing a user-supplied *value* where a document is
expected. `{ "user": req["name"] }` is safe, but if `req["name"]` is itself a
dictionary like `{ "$ne": null }` it becomes an operator and matches every
document. Check the type of anything that came from outside before putting it
in a filter.

## Authentication

SCRAM-SHA-256. The password never crosses the connection in any form: both
sides derive a key with PBKDF2 and prove they know it by exchanging HMACs over
a transcript that includes nonces from both parties, so a captured exchange
cannot be replayed.

The server's half of the exchange is verified too — skipping that check means
authenticating *to* an impostor, which is the half of mutual authentication
that usually gets dropped.

Credentials live in a specific database, `admin` by default. Pointing
`authSource` at the wrong one is the usual cause of "Authentication failed"
with a password that is definitely right.

## Errors

```ez
try { users.insert(user) }
catch (e) {
    when isDuplicateKey(e) { give "already registered" }
    throw e
}
```

The MongoDB code and codeName are attached. A write command can succeed while
individual writes fail — `writeErrors` — and those are raised rather than
silently dropped.

## Connection strings

```ez
db = fromUrl("mongodb://user:password@host:27017/database")
db.open()
```

## BSON on its own

```ez
use "mongodb/bson.ez"
encodeDocument(document)      decodeDocument(bytes, 0)
encodeOrderedDocument(pairs)  ObjectId.fromHex(text)   Binary(bytes)
```

`encodeOrderedDocument` exists because BSON is order-sensitive where a
dictionary is not: a command names itself in its first field, and EZ's `keys()`
preserves neither insertion nor alphabetical order.

## Tests

```
ez test.ez
```

92 assertions. BSON is checked offline; the rest runs against a real server,
because a wire protocol tested against a mock is tested against your own
reading of the specification:

```
docker run -d -p 27018:27017 -e MONGO_INITDB_ROOT_USERNAME=ezuser \
  -e MONGO_INITDB_ROOT_PASSWORD=ezpass mongo:7
```

## License

MIT
