# google

Google Workspace APIs for EZ — Sheets, Drive, Calendar and Gmail.

```ez
use "google"
use "oauth"

token = OAuth2.provider("google", ID, SECRET, REDIRECT).complete(query, session)
g = Google(token)

g.sheets().records(SHEET_ID, "Sheet1!A1:D100")
g.drive().upload("report.csv", csvText, "text/csv")
g.calendar().events("primary")
g.gmail().send("them@example.com", "Subject", "Body")
```

## Install

```
ez install google
```

Depends on `httpx`. Pair it with `oauth`, which is what gets you the token —
this is what to do with it afterwards.

## The token

Pass an `oauth` `Token`, or a bare access-token string. The Token is better: it
knows when it expires, and given the oauth client this package renews it rather
than letting a request fail at an awkward moment.

```ez
g = Google(token).withRefresh(oauthClient)
```

Scopes are granted at consent time, so ask for what you need up front —
`https://www.googleapis.com/auth/spreadsheets`,
`.../auth/drive.file`, `.../auth/calendar`, `.../auth/gmail.send`.

## Sheets

```ez
s = g.sheets()

s.values(id, "Sheet1!A1:C10")     # rows as lists, exactly as Google sends them
s.rows(id, range)                 # the same, padded to a rectangle
s.records(id, range)              # first row as headers, rest as dictionaries
s.update(id, range, rows)         # overwrite
s.append(id, range, rows)         # add to the end
s.clear(id, range)
s.create(title)     s.info(id)    s.sheetNames(id)
```

**Google omits trailing empty cells** rather than padding, so `values` comes
back ragged — a row whose last two cells are blank is shorter than its
neighbours, and indexing into a fixed column reads past the end. `rows` and
`records` handle that.

`update` and `append` default to `USER_ENTERED`, which makes Google parse what
you write the way it would parse typing: `"1/2"` becomes a date and `"=A1+1"` a
formula. Pass `"RAW"` to store text exactly.

## Drive

```ez
d = g.drive()

d.list(query)                     # Drive's search syntax
d.get(fileId)                     # metadata
d.download(fileId)                # contents
d.downloadToFile(fileId, path)
d.upload(name, content, mimeType, parentId)
d.uploadFile(path)
d.createFolder(name, parentId)
d.remove(fileId)
d.shareWithAnyone(fileId)         # makes the file PUBLIC
```

`download` sets `alt=media`, which is what separates contents from metadata —
without it Drive returns the metadata with a 200 and nothing looks wrong.

Uploads are multipart, because a simple upload cannot carry a filename and the
file lands called "Untitled".

## Calendar

```ez
c = g.calendar()
c.calendars()
c.events("primary", { "timeMin": toIso(now()) })
c.createEvent("primary", "Review", start, end)
c.deleteEvent("primary", eventId)
```

Times are RFC 3339, which `datetime`'s `toIso()` produces. `events` expands
recurring events into instances rather than returning the rule.

## Gmail

```ez
m = g.gmail()
m.profile()
m.messages("is:unread from:someone")
m.message(id)
m.send(to, subject, body)
m.send(to, subject, html, { "html": true })
```

`send` builds the RFC 5322 message and base64**url** encodes it — not plain
base64, whose `+` and `/` Gmail rejects.

## Errors

Google's own explanation comes through, plus the `reason` worth branching on:

```
google: GET failed with 403 -- Insufficient Permission (the token is valid but
was not granted the scope this call needs -- add it and have the user consent
again)
```

```ez
catch (e) { when e.reason == "insufficientPermissions" { … } }
```

## Tests

```
ez test.ez
```

67 assertions, no credentials and no network. The HTTP layer is replaced with a
recording transport, so what is checked is what a REST wrapper actually gets
wrong: the URL, the verb, the parameters and the body. Calling the real APIs
would test Google, not this package.

## License

MIT
