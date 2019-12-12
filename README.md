# log_proxy

## What is it?

**log_proxy** is a tiny C utility for log rotation for apps that write their logs to stdout.

This is very useful, specially for [12-factor apps that write their logs to stdout](https://12factor.net/logs).

It can be used to avoid loosing some logs if you use `logrotate` with `copytruncate` feature or to prevent a log file from filling your hard disk.

## Features

- [x] usable as a pipe (`myapp myapp_arg1 myapp_arg2 |log_proxy /log/myapp.log`)
- [x] configurable log rotation suffix with `stftime` placeholders (for example: `.%Y%m%d%H%M%S`)
- [x] can limit the number of rotated files (and delete oldest)
- [x] can rotate files depending on their size (in bytes)
- [x] can rotate files depending on their age (in seconds)
- [x] does not need a specific log directory for a given app (you can have one directory with plenty of different log files from different apps)
- [x] several instances of the same app can log to the same file without issue (example: `myapp arg1 |log_proxy --use-locks /log/myapp.log` and `myapp arg2 |log_proxy --use-locks /log/myapp.log` can run at the same time)
- [ ] configurable action (a command to execute) to run after each log rotation
- [ ] rock solid
- [x] really fast
- [x] do not eat a lot of memory
- [ ] configurable with CLI options as well with env variables
- [ ] usable as a wrapper to capture stdout and stderr (`log_proxy_wrapper --stdout=/log/myapp.stdout --stderr=/log/myapp.stderr -- myapp myapp_arg1 myapp_arg2`)
- [ ] usable as a wrapper to capture stdout and stderr in the same file (`log_proxy_wrapper --stdout=/log/myapp.log --stderr=STDOUT -- myapp myapp_arg1 myapp_arg2`)

## Why this tool?

### Why not using `logrotate` with `copytruncate` feature?

If you use `myapp myapp_arg1 myapp_arg2 >/log/myapp.log 2>&1`for example and if you can't stop easily your app (because it's a critical thing), you can configure `logrotate` with `copytruncate` feature to do the log rotation of `/log/myapp.log` but:

- you may loose a few lines during log rotation (1)
- the rotation is mainly time-based, so you can fill your storage if your app suddently start to be very very verbose

(1), see https://unix.stackexchange.com/questions/475524/how-copytruncate-actually-works

> Please note also that copyrotate has an inherent race condition, in that it's possible that the writer will append a line to the logfile just after logrotate finished the copy and before it has issued the truncate operation. That race condition would cause it to lose those lines of log forever. That's why rotating logs using copytruncate is usually not recommended, unless it's the only possible way to do it.

### Why developping another tool?

After reading: https://superuser.com/questions/291368/log-rotation-of-stdout and http://zpz.github.io/blog/log-rotation-of-stdout/, we reviewed plenty of existing tools (`multilog`, `rotatelogs`, `piper`...).

But none of them was ok with our needed features:

- configurable log rotation on size **AND** age
- no dedicated log directory for an app
- (and) several instances of the same app can log to the same log file without issue

The [piper tool](https://github.com/gongled/piper) was the more close but does not support the last feature (several instances to the same log file).

## Help

```console
$ ./log_proxy --help
Usage:
  log_proxy [OPTION?] LOGNAME  - log proxy

Help Options:
  -h, --help                Show help options

Application Options:
  -s, --rotation-size       maximum size (in bytes) for a log file before rotation (0 => no maximum, default: 104857600 (100MB))
  -t, --rotation-time       maximum lifetime (in seconds) for a log file before rotation (0 => no maximum, default: 86400 (24H))
  -S, --rotation-suffix     strftime based suffix to append to rotated log files (default: .%Y%m%d%H%M%S)
  -n, --rotated-files       maximum number of rotated files to keep including main one (0 => no cleaning, default: 5)
  -m, --use-locks           use locks to append to main log file (useful if several process writes to the same file)
  -f, --fifo                if set, read lines on this fifo instead of stdin
  -r, --rm-fifo-at-exit     if set, drop fifo at then end of the program (you have to use --fifo option of course)
```

## FIXME

to be continued
