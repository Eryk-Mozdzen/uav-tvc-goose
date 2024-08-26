#!/bin/bash

type=$1
file=$2

if [ "$type" == "swd" ]; then
    st-flash --connect-under-reset write $file 0x08000000
elif  [ "$type" == "ota" ]; then
    arm-none-eabi-gdb -q $file << GDBSCRIPT
        target extended-remote 10.42.0.108:2022
        monitor swdp_scan
        attach 1
        load
        monitor reset
        detach
        quit
GDBSCRIPT
echo ""
else
	echo "unknown program method"
fi
