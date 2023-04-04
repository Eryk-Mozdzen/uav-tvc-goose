#!/bin/bash

type=$1
file=$2

if [ "$type" == "stlink" ]; then
	st-flash --connect-under-reset write $file 0x08000000
elif [ "$type" == "dfu" ]; then
	dfu-util -a 0 --dfuse-address 0x08000000:leave --download $file
else
	echo "unknown program method"
fi
