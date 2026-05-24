#!/bin/bash

STM32_Programmer_CLI -c port=swd -w nrf24.bin 0x08000000 -rst
