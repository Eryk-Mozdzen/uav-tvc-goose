
file=$1

#st-flash --connect-under-reset write $file 0x08000000

dfu-util -a 0 --dfuse-address 0x08000000:leave --download $file
