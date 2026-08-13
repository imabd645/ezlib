# archive

Zip and gzip for EZ.

```ez
use "archive"

files = Zip.readAll("export.zip")          # name -> text
Zip.write("out.zip", { "notes.txt": "hello" })

text = Gzip.decompressText(readFile("page.html.gz"))
writeFile("out.gz", Gzip.compressText("hello"))
```

## Install

```
ez install archive
```

No dependencies, and **nothing to install**. Windows ships no raw DEFLATE — the
Compression API in `Cabinet.dll` does MSZIP, XPRESS and LZMS, and ntdll's
`RtlDecompressBuffer` does LZNT1 and XPRESS, none of which is what a `.zip` or
`.gz` actually contains. So this package brings its own decompressor.

## What works, honestly

**Reading is complete.** Stored and deflated entries both work, which covers
every zip and gzip you are likely to be handed. Verified against archives
produced by Python's `zipfile` and `gzip`, including binary content and all 256
byte values.

**Writing produces valid, universally readable archives but does not
compress.** Zip entries are STORED; a gzip member is written as stored DEFLATE
blocks. Output is larger than the input by a few bytes per 64KB. Files written
here open in Explorer, `unzip`, Python and 7-Zip — checked, not assumed.

Compressing as well needs an encoder this package does not have yet. Writing a
file that *claimed* to be deflated but was not would be worse than saying this
plainly.

**Speed.** Decompression is EZ, not C. Fine for a config bundle, a data export,
an uploaded document; not for a multi-gigabyte archive.

## Zip

```ez
Zip.read(path)              # ZipEntry list
Zip.readAll(path)           # name -> text, directories skipped
Zip.readBytes(bytes)        # from memory
Zip.names(path)
Zip.write(path, files)      # files: name -> text or byte array
Zip.writeBytes(files)       # to memory
Zip.isZip(bytes)
```

```ez
entry.name        entry.text()      entry.bytes
entry.size()      entry.method      entry.isDirectory()
```

Archives are read through the **central directory** at the end of the file
rather than by walking local headers from the front. That is what the format
intends: the directory is authoritative, it is how a zip with a prepended
self-extractor still reads, and a local header may not know its own sizes when
the entry was written as a stream.

## Gzip

```ez
Gzip.decompress(bytes)      Gzip.decompressText(text)
Gzip.compress(bytes, name)  Gzip.compressText(text)
Gzip.isGzip(bytes)
```

The trailer's length and CRC32 are both checked. A truncated download or a
corrupted byte is rejected rather than returning partial data — that is the
whole reason gzip carries a trailer.

## CRC32

```ez
use "archive/crc32.ez"
crc32Text("123456789")      # 3421780262
crc32Bytes(bytes)
```

Checked against the published check value. It is a checksum, not a hash: it
catches accidental corruption, not deliberate tampering.

## DEFLATE directly

```ez
use "archive/inflate.ez"
inflateBytes(rawDeflateBytes)
```

RFC 1951: stored, fixed-Huffman and dynamic-Huffman blocks.

## Tests

```
ez test.ez
```

41 assertions. The fixtures are real files produced by a real compressor,
embedded as bytes — decompressing something this package compressed would only
prove it agrees with itself, and the point is to agree with everyone else.

## License

MIT
