#!/bin/bash

#LDFLAGS=-Wl,-O2 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,--disable-new-dtags -Wl,--gc-sections -Wl,-rpath,$PREFIX/lib -Wl,-rpath-link,$PREFIX/lib -L$PREFIX/lib
# For some reason, conda compiler default LDFLAGS (above) doesn't work with log_proxy. So we set LDFLAGS to nothing.
export LDFLAGS=
make all
make install
