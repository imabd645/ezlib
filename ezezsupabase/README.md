# EZ Supabase (`ezsupabase`)

A native, HTTP-based client for interacting with Supabase PostgreSQL databases directly from EZ Language. Because it uses Supabase's PostgREST API over standard HTTP, **you do not need to download or install any ODBC drivers!**

## Connecting

You only need your Supabase **Project URL** and an **API Key** (either the public `anon` key or the `service_role` key).

```javascript
use "ezsupabase"

client = SupabaseClient("https://aws-1-ap-south-1.pooler.supabase.com", "YOUR_API_KEY")
```

## API Reference

### `client.from("table_name").select(columns)`
Fetches rows from the database. Passes `nil` to `columns` to fetch all columns (`*`), or provide a comma-separated string like `"id, name"`.
```javascript
users = client.from("users").select(nil)
get user in users {
    out user["name"]
}
```

### `client.from("table_name").insert(data)`
Inserts a single dictionary object (or an array of objects) into the table. Returns the inserted rows.
```javascript
newRow = client.from("users").insert({
    "name": "Abdullah",
    "email": "abdullah@example.com"
})
```

### `client.from("table_name").update(matchFilter, data)`
Updates rows that exactly match the key-value pairs in `matchFilter` with the new fields in `data`.
```javascript
# Update user where id = 5
client.from("users").update({"id": 5}, {"name": "New Name"})
```

### `client.from("table_name").delete(matchFilter)`
Deletes rows that match the given filter.
```javascript
client.from("users").delete({"id": 5})
```
