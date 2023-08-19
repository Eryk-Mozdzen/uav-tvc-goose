#!/bin/bash

type=$1
file=$2

if [ "$type" == "stlink" ]; then
	st-flash --connect-under-reset write $file 0x08000000
elif [ "$type" == "dfu" ]; then
	dfu-util -a 0 --dfuse-address 0x08000000:leave --download $file
elif  [ "$type" == "ota" ]; then
	arm-none-eabi-gdb -q $file << GDBSCRIPT
		target extended-remote 192.168.102.29:2022
		load
		monitor reset
		detach
		quit
GDBSCRIPT
	echo ""
else
	echo "unknown program method"
fi
