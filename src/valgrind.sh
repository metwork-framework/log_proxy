#!/bin/bash

function usage()
{
	echo "usage: valgrind.sh /path/program/test [OPTIONS]"
}

if test $# -lt 1; then
	usage
	exit
fi

which valgrind >/dev/null 2>&1
if test $? -ne 0; then
  echo "VALGRIND NOT FOUND"
  exit 1
fi

if test -f valgrind.suppressions; then
  SUPPS="--suppressions=valgrind.suppressions"
else
  SUPPS=""
fi

echo "MEMORY ERRORS TESTS..."
valgrind --error-exitcode=1 ${SUPPS} "$@"
if test $? -eq 0; then
  echo "OK"
else
  echo '=> MEMORY ERRORS DETECTED'
  exit 1
fi

echo "TEST DES FUITES MEMOIRES..."
valgrind --tool=memcheck --leak-check=full --show-possibly-lost=no ${SUPPS} "$@" 2>&1 |tee /tmp/valgrind.out
N=$(cat /tmp/valgrind.out |grep -c "definitely lost: 0 bytes in 0 blocks")
if test "${N}" -gt 0; then
  echo "OK"
else
  echo "=> MEMORY LEAKS DETECTED"
  exit 1
fi
