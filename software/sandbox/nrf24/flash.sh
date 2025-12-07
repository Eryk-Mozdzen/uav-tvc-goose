#!/bin/bash

STM32_Programmer_CLI -c port=swd -w $1 0x08000000 -rst
