# CHANGELOG



## v0.0.1 (2019-12-26)

### New Features
- log_proxy first version
- add a mode to read log lines from fifo instead of stdin
- first try with log_proxy_wrapper
- add optional environment variables to override default values
- add non blocking mode to lock_control_file
- add other tests on control.c


### Bug Fixes
- post test fixes
- don't leak fifo if SIGTERM or SIGINT
- fix initialization value and GOptionArg type for long parameters





