# geohash

Compress a latitude/longitude pair into a short base-32 string, decode it back,
and find the cells around it. Pure EZ, no dependencies.

```ez
use "geohash"

ghEncode(57.64911, 10.40744, 11)   # "u4pruydqqvj"

ghDecode("u4pruydqqvj")
# { lat: 57.649111, lng: 10.407440, latErr: 0.000001, lngErr: 0.000001,
#   minLat: ..., maxLat: ..., minLng: ..., maxLng: ... }

ghNeighbors("9q9hvu")["n"]         # "9q9hvv"
```

## Why a geohash

A geohash interleaves the bits of latitude and longitude and encodes the result
in base 32. The useful consequence is the **prefix property**: two nearby points
share a long prefix, so "are these two points in the same city" becomes a
`startsWith` instead of a distance calculation. Databases and key-value stores
use this to turn a radius search into a range scan.

Each character halves the cell in both dimensions. `precision` runs 1..12:

| chars | cell size (approx) |
|-------|--------------------|
| 1     | 5,000 km           |
| 5     | 4.9 km             |
| 7     | 153 m              |
| 9     | 4.8 m              |
| 12    | 3.7 cm             |

## API

| Function | Returns |
|----------|---------|
| `ghEncode(lat, lng, precision = 12)` | geohash string |
| `ghDecode(hash)` | `{ lat, lng, latErr, lngErr, minLat, maxLat, minLng, maxLng }` |
| `ghBoundingBox(hash)` | `{ minLat, maxLat, minLng, maxLng }` |
| `ghNeighbors(hash)` | `{ n, ne, e, se, s, sw, w, nw }` |
| `ghValidate(hash)` | `true` / `false` |

`ghDecode` returns the cell **centre** and the half-extent of the cell in each
axis (`latErr`, `lngErr`), so the cell spans `lat ± latErr` and `lng ± lngErr`.

## Errors

Out-of-range coordinates, out-of-range precision, and characters outside the
geohash alphabet all throw. The alphabet is the standard base-32 with `a`, `i`,
`l` and `o` removed (so a hash can never spell a word), which is why uppercase
letters and `a` are rejected by `ghValidate`.

## Notes

- Latitude and longitude are treated as plain numbers; the package does no
  geocoding (address → coordinate is a different problem).
- Distance between two points is deliberately out of scope — see the notes in
  the repo about a future `geo` package for haversine/vincenty.
