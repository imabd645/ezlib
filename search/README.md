# search

Full-text search for EZ. Meilisearch and Elasticsearch clients.

```ez
use "search"

engine = Meili("http://localhost:7700", MASTER_KEY)
books = engine.index("books")

books.add([{ "id": 1, "title": "The Left Hand of Darkness" }])
books.waitForPending()                    # indexing is asynchronous
hits = books.find("darkness")
```

## Install

```
ez install search
```

Depends on `httpx`.

## Two engines, two clients

They are kept separate rather than hidden behind one abstraction. Their query
languages are genuinely different — Meilisearch is a typo-tolerant
instant-search engine, Elasticsearch is a query DSL over an inverted index —
and a lowest-common-denominator wrapper would expose neither well. What *is*
shared is the shape: `index(name)` gives you something you can add to, search
and delete from.

`text` gives you fuzzy matching over a list you already hold in memory. This is
for when the corpus does not fit there.

## Meilisearch

```ez
m = Meili(url, apiKey)

m.createIndex("books", "id")              # returns a task id
m.indexes()      m.health()      m.version()

books = m.index("books")
books.add(documents)                      # add or replace
books.update(documents)                   # merge into existing
books.search(query, options)              # the full response
books.find(query, options)                # just the hits
books.document(id)     books.documents(limit, offset)
books.remove(id)       books.clear()      books.count()
books.setFilterable(["author", "year"])
books.setSortable(["year"])
```

**Indexing is asynchronous.** Every write returns a task id, and reading
immediately afterwards sees the old state. That is the single thing that makes
Meilisearch code look broken when it is not:

```ez
books.add(documents)
books.waitForPending()                    # or m.waitForTask(id)
```

`waitForTask` throws if the task failed, because a silently failed index write
is worse than a slow one.

Filters and sorts only work on fields you have declared with `setFilterable` /
`setSortable` — Meilisearch refuses others, and that is the first thing
everyone trips over.

## Elasticsearch

Works with OpenSearch too.

```ez
es = Elastic(url, username, password)      // or .withApiKey(key)

es.createIndex("books", mappings)
es.indexes()     es.info()

idx = es.index("books")
idx.add(document, id)
idx.addMany(documents, "id")               # bulk, one round trip
idx.refresh()                              # make writes visible now
idx.search(text, options)                  # multi_match across fields
idx.find(text)                             # just the _source values
idx.query(body)                            # the raw DSL
idx.document(id)   idx.remove(id)   idx.count()
```

Elasticsearch indexes asynchronously too, but unlike Meilisearch it offers a
synchronous `refresh()`.

## Errors

Both engines' own explanations come through — Meilisearch's `message` and
Elasticsearch's `error.reason` — rather than a generic failure:

```
SearchError(400): Attribute `year` is not filterable. Available filterable
attributes are: `author`.
```

## Tests

```
ez test.ez
```

51 assertions. The offline section covers the compact JSON encoder that the
bulk endpoint depends on; the live sections run against a real Meilisearch and
a real Elasticsearch and skip with a reported count when either is absent.

## License

MIT
