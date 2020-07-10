#!/bin/bash

# paranoid mode
set -eu

# Check if we are root (or if we used FORCE argument)
IDU=$(id -u)
if test "${IDU}" != "0"; then
  if test "${1:-}" != "FORCE"; then
    echo "ERROR: you must run this script as root user"
    echo "       (or use FORCE argument if you know exactly what you are doing)"
    exit 1
  fi
fi

echo "Uninstalling..."
rm -f /usr/local/bin/log_proxy
rm -f /usr/local/bin/log_proxy_wrapper
echo "Done"
