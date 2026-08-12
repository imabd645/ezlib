# args — command-line argument parsing for EZ

> **Import:** `use "args"`
> **Install:** `ez install args`
> **Requires:** an interpreter with the `argv` global

```ez
use "args"

cli = Args("greet", "Greets someone")
cli.positional("name", "Who to greet").required()
cli.flag("loud", "l", "Shout it")
cli.option("times", "t", "How many times").number().withDefault(1)

opts = cli.parse(argv)

repeat i = 1 to opts["times"] {
    message = "Hello, " + opts["name"]
    when opts["loud"] { message = upper(message) }
    out message
}
```

```
$ ez greet.ez Ali -lt 2
HELLO, ALI
HELLO, ALI
```

---

## One declaration, three jobs

The same declaration drives parsing, validation and `--help`. Help written by
hand always drifts out of step with what the program actually accepts; help
generated from the parser cannot.

```
$ ez greet.ez --help
Usage: greet [options] <name>

Greets someone

Arguments:
  name                     Who to greet (required)

Options:
  -l, --loud               Shout it
  -t, --times <TIMES>      How many times [default: 1]
  -h, --help               Show this message
```

## Declaring

| Call | Accepts |
| --- | --- |
| `.flag(name, short, help)` | `--verbose` / `-v`, no value |
| `.option(name, short, help)` | `--output file` / `-o file` |
| `.positional(name, help)` | An argument identified by position |

Pass `""` for `short` to omit the single-letter form.

### Modifiers

Each applies to the most recent declaration and returns the parser, so they
chain:

| Modifier | Effect |
| --- | --- |
| `.required()` | Missing is an error |
| `.withDefault(v)` | Value when absent; implies not required |
| `.number()` | Parse as a number; a non-number is an error |
| `.boolean()` | Accept `true/false/yes/no/1/0/on/off` |
| `.oneOf(list)` | Restrict to these values |
| `.many()` | Collect every occurrence into a list |
| `.placeholder(text)` | Name shown in help instead of the uppercased name |
| `.versioned(text)` | Enable `--version` |
| `.withoutAutoHelp()` | Handle `--help` yourself |

## Syntax accepted

```
--output file        --output=file       -o file        -ofile
-v                   -abc                (clustered short flags)
--                   everything after this is data, not options
```

**Clustered flags** work as expected: `-lt 2` is `-l` plus `-t 2`.

**The `--` separator** exists so a program can accept a value that looks like
an option. Without it, `myprog -- --verbose` could never pass the literal
string `--verbose` through.

Options and positionals interleave freely — `-q value` and `value -q` both
work.

## Parsing

```ez
opts = cli.parse(argv)
```

`parse` does what a program wants: prints help and exits 0 for `--help`,
prints the errors and exits 2 for bad input, otherwise returns the values.

For anything else — tests, a custom error format, a REPL — use `tryParse`,
which **never throws and never exits**:

```ez
result = cli.tryParse(argv)

result["ok"]                # false when there were errors
result["values"]            # what was parsed
result["errors"]            # every problem, not just the first
result["extra"]             # positionals beyond those declared
result["helpRequested"]     # --help was given
result["versionRequested"]  # --version was given
```

**Every error is reported, not just the first.** Fixing one problem at a time
across repeated runs is a poor experience.

## About `argv`

`argv` is a global list of the arguments given to the script. Interpreter
flags are not included:

```
ez app.ez --trace foo        # argv is ["foo"]; --trace goes to the interpreter
ez app.ez -- --trace         # argv is ["--trace"]
```

`scriptName` holds the path the script was invoked as, for usage messages.

Both are always defined — `argv` is a list even with no arguments, so
`len(argv)` is always safe. A bundled `.exe` receives its own command line the
same way.

## Notes

- **`parse` calls `exit`.** That is right for a program and wrong for a
  library; use `tryParse` if you need control.
- **List defaults are copied per parse.** Sharing the declared `[]` would let
  one run's values leak into the next — worth knowing if you write a parser
  that behaves like this yourself.
- **No subcommands** (`git commit` style). Parse the first positional
  yourself and dispatch to a second `Args` with the remainder.
- **No config-file or environment fallback.** Combine with `env` or `toml` if
  you want options to come from more than the command line.

## Testing

```
ez test.ez
```

80 tests covering every syntax form, clustering, type coercion and its
failures, choices, defaults, required checks, repeated values, the `--`
separator, help generation, and the shared-default bug described above.

## Structure

| File | Role |
| --- | --- |
| `main.ez` | The `Args` model: declaring, parsing, help generation |
| `spec.ez` | Option records, labels, type coercion, choice checking |

## License

MIT
