# EZ ODBC (`ezodbc`)
The universal database connector for EZ Language.

`ezodbc` is a production-ready, fully native FFI wrapper around Windows ODBC (`odbc32.dll`). It enables EZ to connect to **any** SQL database on the planet (MySQL, PostgreSQL, Azure SQL Server, Oracle, SQLite) as long as you have the appropriate ODBC driver installed on your machine.

**The required `odbc32.dll` is already shipped inside the `dll` folder for guaranteed compatibility and portability!**

## Connecting to a Database
You connect using standard ODBC Connection Strings.

### Example: MySQL
```javascript
use "ezodbc"

# Make sure you have the MySQL ODBC Driver installed on your machine
db = Database("Driver={MySQL ODBC 8.0 Driver};Server=localhost;Database=mydb;User=root;Password=pass;")

# Run a SELECT query
users = db.query("SELECT id, username, email FROM users")
get u in users {
    out u["username"] + " (" + u["email"] + ")"
}

db.close()
```

### Example: PostgreSQL
```javascript
use "ezodbc"

db = Database("Driver={PostgreSQL Unicode};Server=localhost;Port=5432;Database=mydb;Uid=root;Pwd=pass;")
rows = db.query("SELECT * FROM employees")
```

### Example: Azure SQL / Microsoft SQL Server
```javascript
use "ezodbc"

db = Database("Driver={ODBC Driver 17 for SQL Server};Server=tcp:myserver.database.windows.net,1433;Database=mydb;Uid=user;Pwd=pass;Encrypt=yes;TrustServerCertificate=no;")
```

## API Reference

### `Database(connectionString)`
Initializes a new database connection. It will throw an error if the connection string is invalid or if the database is unreachable.

### `db.query(sql)`
Executes a `SELECT` statement and returns an array of dictionaries representing the rows. All columns are parsed as strings or `nil` (for NULL values).

### `db.execute(sql)`
Executes an `INSERT`, `UPDATE`, `DELETE`, or `CREATE TABLE` statement. Returns `true` if successful.

### `db.close()`
Disconnects from the database and frees all FFI handles. Memory leaks will occur if you do not call this when finished.
