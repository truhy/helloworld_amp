#!/bin/bash

set -e
function cleanup {
	rc=$?
	# If error and shell is child level 1 then stay in shell
	if [ $rc -ne 0 ] && [ $SHLVL -eq 1 ]; then exec $SHELL; else exit $rc; fi
}
trap cleanup EXIT

if [ -z "${BM_HOME_PATH+x}" ]; then
	source ../scripts-env/env-linux.sh
fi

cd $BM_HOME_PATH

# Determine build from input argument
if [ $1 = "debug" ]; then
	app1_elf="DebugApp1/$BM_PROGRAM_NAME1".elf
	app2_elf="DebugApp2/$BM_PROGRAM_NAME2".elf
	ubootspl="$BM_SRC_PATH1/startup/u-boot-spl-nocache"
else
	app1_elf="ReleaseApp1/$BM_PROGRAM_NAME1".elf
	app2_elf="ReleaseApp2/$BM_PROGRAM_NAME2".elf
	ubootspl="$BM_SRC_PATH1/startup/u-boot-spl"
fi

# Find elf entry point
user_entry=$(arm-none-eabi-readelf -h $app1_elf | grep "Entry point" | cut -d : -f 2)
#user_entry=0x00100040

# Reset SoC HPS, load and execute U-Boot SPL elf
openocd -f interface/altera-usb-blaster2.cfg -f target/altera_fpgasoc_de.cfg -c "init; halt; c5_reset; halt; c5_spl $ubootspl; sleep 200; halt; arm core_state arm; load_image $app2_elf; load_image $app1_elf; resume $user_entry; shutdown"