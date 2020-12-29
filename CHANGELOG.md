# CHANGELOG

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


