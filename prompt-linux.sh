#!/bin/bash

source scripts-env/env-linux.sh
source prompt-info.sh

# If shell is child level 1 (e.g. Run as a Program) then stay in shell
if [ $SHLVL -eq 1 ]; then exec $SHELL; fi
