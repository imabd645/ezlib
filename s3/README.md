# s3

S3-compatible object storage for EZ. AWS Signature V4, the operations you
actually use, presigned URLs, and presets for R2, Spaces and MinIO.

```ez
use "s3"

store = S3(ACCESS_KEY, SECRET_KEY, "eu-west-1").bucket("my-uploads")

store.putFile("avatars/1.png", "local.png")
store.presignGet("avatars/1.png", 3600)        # a link anyone can use for an hour
```

Files were the gap that stopped an EZ web app being finishable. `orm` holds the
row; this holds the thing the row points at.

## Install

```
ez install s3
```

Depends on `httpx` and `crypto`.

## Providers

S3's protocol is the one every object store speaks:

```ez
S3(key, secret, "eu-west-1")                   # AWS
r2("account-id", key, secret)                  # Cloudflare R2
spaces("fra1", key, secret)                    # DigitalOcean Spaces
minio("localhost:9000", key, secret)           # MinIO, or anything self-hosted
S3(key, secret, region).withEndpoint("s3.eu-central-003.backblazeb2.com")
```

A custom endpoint switches to path-style addressing, because a self-hosted
server has no wildcard DNS and `bucket.localhost:9000` does not resolve. Say
`.virtualHosted()` or `.pathStyle()` if you need to choose explicitly.

From the environment, the way every AWS tool reads it:

```ez
store = fromEnv().bucket("my-uploads")         # AWS_ACCESS_KEY_ID, …
```

## Objects

```ez
store.put(key, body, options)                  # options: contentType, acl,
store.putFile(key, path, options)              #   storageClass, cacheControl,
                                               #   metadata
store.get(key)                                 # contents, or nil if absent
store.getToFile(key, path)
store.head(key)                                # metadata only, or nil
store.exists(key)
store.delete(key)
store.copy(from, to)
store.move(from, to)
```

`get` returns `nil` for a missing object rather than throwing — that is an
ordinary answer, and 404 is the one status you nearly always want to handle
rather than propagate. Everything else throws.

`delete` returns `true` when the delete was accepted. It cannot tell you
whether anything was actually there: S3 answers 204 either way, and claiming
otherwise would be a lie.

Content type is guessed from the extension unless you set it. That matters more
than it looks — the wrong one makes a browser download an image instead of
displaying it, and it cannot be changed later without rewriting the object.

## Listing

```ez
page = store.list("photos/", { "maxKeys": 100 })
page["objects"]        # [{ key, size, etag, lastModified, storageClass }, …]
page["truncated"]      # is there more
page["nextToken"]      # pass back as options.token

store.listAll("photos/")                       # every object, paging for you
store.keys("photos/")                          # just the names
store.folder("photos")                         # immediate children only
```

`listAll` will happily return a million objects from a bucket with a million
objects. Use `list` when that is a possibility.

## Presigned URLs

```ez
store.presignGet(key, 3600)                    # download link
store.presignPut(key, 900)                     # upload link
```

This is how a browser uploads or downloads directly, without the file passing
through your server at all. The link carries its own signature and needs no
credentials — the secret key is never in it.

AWS caps these at seven days, so a longer expiry is refused here rather than by
the server after you have handed the link out.

## Signing

Every request is signed with AWS Signature V4. It is unforgiving: one wrong
byte anywhere in the canonical request produces a completely different
signature, and the server's only reply is `SignatureDoesNotMatch` with no
indication of which part was wrong.

So each stage is built separately and exposed, and the test suite checks each
one against known-good values:

```ez
use "s3/sign.ez"

canonicalRequest(method, path, params, headers, payloadHash)
stringToSign(timestamp, dateStamp, region, service, canonical)
signingKey(secret, dateStamp, region, service)
sign(secret, dateStamp, region, service, toSign)
```

If a request is being rejected, print those in order and compare with what the
server says it expected — the mismatch is always visible at exactly one stage.

The secret key never touches the wire. It only ever seeds the HMAC chain that
produces the per-day, per-region, per-service signing key, which is why a
leaked signature authorises one request and stops working tomorrow.

## Errors

S3 explains itself in XML, and the explanation is far more useful than the
status — `SignatureDoesNotMatch` and `NoSuchBucket` are both refusals and mean
entirely different things. Both come through:

```
s3: GET photos/1.png failed with 403: SignatureDoesNotMatch -- The request
signature we calculated does not match the signature you provided (check the
secret key, the region, and that the machine clock is within 15 minutes of
real time)
```

The three that account for most first-run problems — a wrong secret, a bucket
in another region, and a clock more than fifteen minutes out — get that extra
sentence, because none of the official messages say what to check.

## Binary

Bodies are byte-exact end to end. That is not a nicety here: the payload hash
is part of the signature, so a body that is not byte-exact produces a signature
the server rejects. The test suite fetches a real gzip file and checks its
SHA-256 against the published checksum.

## API

Also exported: `guessContentType(key)`, and from `s3/sign.ez` the signing
stages plus `presign`, `encodePath`, `canonicalQuery`, `timestampFor`,
`nowTimestamp`; from `s3/xml.ez` the extractors `firstTag`, `allTags`,
`blocks`, `escapeXml`, `unescapeXml`.

`s3/xml.ez` is deliberately not a general XML parser — the `xml` package is
that. Reading four element names out of a machine-generated document whose
shape has been fixed for fifteen years does not justify a dependency.

## Tests

```
ez test.ez
```

106 assertions, no credentials needed. The signing vectors were produced by an
independent implementation over the same inputs, and the signing key is AWS's
own published constant. The live section makes one deliberately unauthorised
request to real S3 to prove the transport and the XML error path work end to
end; if the network is down it is skipped and the skip is reported.

## License

MIT
