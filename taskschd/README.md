# taskschd

Windows Task Scheduler for EZ. Create, query, run and delete scheduled tasks.

```ez
use "taskschd"

ts = connect()

ts.create("NightlyBackup", "C:/scripts/backup.bat",
          { "type": "DAILY", "starttime": "02:00" })

task = ts.query("NightlyBackup")
out task.status                     // "Ready"
out task.nextRunTime                // "8/13/2026 2:00:00 AM"

task.run()                          // start it now
task.delete()
```

## Install

```
ez install taskschd
```

Depends on `os` (to run the command) and `csv` (to parse the output).

## How it works

This drives `schtasks.exe`, the command-line front end to the Task Scheduler
that ships with Windows. It does not use the COM API. That means:

- It works with no registration, no admin rights for ordinary user-level tasks,
  and no native module.
- What you can express is what `schtasks` accepts. Trigger types are its
  vocabulary, not the richer COM one.
- Task names and actions are quoted before being passed along.

## Creating

```ez
ts.create(name, action, trigger, options)
```

`action` is the command to run. `trigger` and `options` may be omitted.

### Triggers

```ez
{ "type": "DAILY", "starttime": "02:00" }
{ "type": "WEEKLY", "days": "MON,WED,FRI", "starttime": "09:00" }
{ "type": "MINUTE", "interval": 15 }
{ "type": "ONSTART" }
{ "type": "MONTHLY", "months": "JAN,APR,JUL,OCT", "startdate": "2026/01/01" }
```

| key | meaning |
|---|---|
| `type` | `MINUTE` `HOURLY` `DAILY` `WEEKLY` `MONTHLY` `ONCE` `ONSTART` `ONLOGON` `ONIDLE` |
| `interval` | how often, for the repeating types |
| `days` | `MON,TUE,…` for `WEEKLY` |
| `months` | `JAN,FEB,…` for `MONTHLY` |
| `starttime` / `endtime` | `HH:mm` |
| `startdate` / `enddate` | `yyyy/MM/dd` |

### Options

```ez
ts.create(name, action, trigger, {
    "runLevel": "highest",          // elevated; "limited" is the default
    "user": "SYSTEM",
    "password": secret
})
```

`create` always passes `/F`, so it **overwrites** an existing task of the same
name rather than failing.

## The task object

`query` returns a `ScheduledTask`, or throws `TaskNotFoundError`.

```ez
task.name           task.status          task.nextRunTime
task.lastRunTime    task.lastResult      task.creator
task.rawDetails                          // the parsed CSV row

task.run()          task.end()           task.delete()
task.refresh()                           // re-read from the scheduler
```

> `query` uses the default CSV output, which carries only three columns —
> TaskName, Next Run Time and Status. So `lastRunTime`, `lastResult` and
> `creator` come back `nil`. They are there for a future verbose query rather
> than being values you can rely on today.

## By name, without the object

```ez
ts.runTask(name)
ts.endTask(name)
ts.delete(name)
```

## Scoped use

```ez
with_scheduler(|ts| {
    ts.create(…)
})
```

## Backslashes in task paths

A task inside a folder is named `\Folder\TaskName`. EZ string literals **drop**
`\\` — `"\\Folder\\Task"` becomes `FolderTask` — so build the separator
explicitly:

```ez
SEP = chr(92)
ts.query(SEP + "Microsoft" + SEP + "Windows" + SEP + "Defrag" + SEP + "ScheduledDefrag")
```

Top-level task names need none of this.

## Errors

| | |
|---|---|
| `TaskNotFoundError` | no task by that name |
| `AccessDeniedError` | the operation needs elevation |
| `TaskschdError` | anything else `schtasks` reported |

## License

MIT
