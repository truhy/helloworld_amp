#!/bin/bash

set -e
function cleanup {
	rc=$?
	if [ $rc -ne 0 ] && [ $SHLVL -eq 1 ]; then exec $SHELL; else exit $rc; fi
	# If error and shell is child level 1 then stay in shell
}
trap cleanup EXIT

./runelf.sh
