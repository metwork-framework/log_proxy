# CHANGELOG

## v0.7.4 (2024-11-05)

### Bug Fixes

- break out in case of write problem (fix #44)

## v0.7.3 (2024-02-12)

### Bug Fixes

- add more examples and correct some confusing wording in cli options' descriptions (#43)

## v0.7.2 (2024-02-09)

### Bug Fixes

- pass missing options -c, -o and -g from wrapper to logger process

## v0.7.1 (2024-02-09)

### Bug Fixes

- print more descriptive error for strftime() failure on timestamp-prefix (#41)

## v0.7.0 (2024-02-09)

### Bug Fixes

- build log_proxy command in wrapper as an argv array, don't use gshell parser
- cleanup glib includes, add some missing libc ones

## v0.6.0 (2024-02-09)

### New Features

- add -T/--timestamps (env LOGPROXY_TIMESTAMPS) option to prepend strftime to each line

### Bug Fixes

- pass -d/--log-directory command-line option from wrapper to logger process
- fix signal_handler for SIGINT

## v0.5.2 (2022-02-21)

### New Features

- add optional environment variable LOGPROXY_CHMOD to change logf… (#32)
- add optional environment variable LOGPROXY_CHMOD to change logf… (#33)

## v0.5.1 (2021-10-14)

### Bug Fixes

- centos8 memory fix (#31)

## v0.5.0 (2021-08-16)

### Bug Fixes

- potential fix for malloc deadlock in some corner cases (#30)

## v0.4.4 (2021-01-21)

### Bug Fixes

- return a !=0 status code if we can't launch the program

## v0.4.3 (2021-01-07)

- No interesting change

## v0.4.2 (2020-12-28)

### Bug Fixes

- fix installer cleaning

## v0.4.1 (2020-12-28)

### Bug Fixes

- fix missing options in log_proxy_wrapper (#26)

## v0.4.0 (2020-12-06)

### New Features

- add a chmod/chown/chgrp option (#22)

## v0.3.1 (2020-07-27)

### Bug Fixes

- fix --fifo-tmp-dir documentation

## v0.3.0 (2020-07-22)

### New Features

- add fifo-tmp-dir option to log_proxy_wrapper

## v0.2.3 (2020-07-10)

- No interesting change

## v0.2.2 (2020-07-10)

- No interesting change

## v0.2.1 (2020-07-10)

- No interesting change

## v0.2.0 (2020-07-10)

### New Features

- add option for static compiling
- releases are now static builds
- add install script

## v0.1.1 (2020-05-04)

- No interesting change

## v0.1.0 (2020-04-29)

### Bug Fixes

- fix a potential deadlock in some corner cases

## v0.0.9 (2020-04-16)

- No interesting change

## v0.0.8 (2020-03-23)

### New Features

- use github action

## v0.0.7 (2020-03-23)

- No interesting change

## v0.0.6 (2020-03-18)

### New Features

- force rotation-size to 90% of rlimit-fsize (if set)

## v0.0.5 (2020-02-24)

### New Features

- prefix control files names by "." (dot)

## v0.0.4 (2020-01-28)

- No interesting change

## v0.0.3 (2020-01-27)

### New Features

- add possibility to specify a log directory

## v0.0.2 (2020-01-03)

### Bug Fixes

- misuse of the g_build_path function

## v0.0.1 (2019-12-26)

### New Features

- log_proxy first version
- add a mode to read log lines from fifo instead of stdin
- first try with log_proxy_wrapper
- add optional environment variables to override default values
- add other tests on control.c

### Bug Fixes

- post test fixes
- don't leak fifo if SIGTERM or SIGINT
- fix initialization value and GOptionArg type for long parameters


