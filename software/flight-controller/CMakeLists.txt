cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "arm-none-eabi.cmake")

project(flight-controller C ASM)

set(TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${TARGET}
    "../common/math/utils.c"
	"../common/protocol/protocol.c"

	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c"
	"Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c"

	"Core/Startup/startup_stm32u535retx.s"
    "Core/Src/main.c"
    "Core/Src/stm32u5xx_hal_msp.c"
    "Core/Src/stm32u5xx_it.c"
    "Core/Src/syscalls.c"
    "Core/Src/sysmem.c"
    "Core/Src/system_stm32u5xx.c"

	"app/app_main.c"
)

target_compile_definitions(${TARGET} PRIVATE
    STM32U535xx
)

target_include_directories(${TARGET} PRIVATE
    "../common/math"
	"../common/protocol"

    "Drivers/CMSIS/Include"
    "Drivers/CMSIS/Device/ST/STM32U5xx/Include"
	"Drivers/STM32U5xx_HAL_Driver/Inc"
    "Core/Inc"
)

target_compile_options(${TARGET} PRIVATE
	$<$<COMPILE_LANGUAGE:ASM>:
		-Os -g

		-mthumb
		-mhard-float
		-mcpu=cortex-m33
		-mfpu=fpv4-sp-d16
		-specs=nano.specs
	>

	$<$<COMPILE_LANGUAGE:C>:
		-Os -g
		-std=c17

		-Wall
		-Wpedantic
		-Wdouble-promotion

		-ffunction-sections
		-fdata-sections

		-mthumb
		-mhard-float
		-mcpu=cortex-m33
		-mfpu=fpv4-sp-d16
		-specs=nano.specs
	>
)

target_link_options(${TARGET} PRIVATE
	-T "${CMAKE_SOURCE_DIR}/STM32U535RETX_FLASH.ld"
	-Wl,--gc-sections

	-mthumb
	-mhard-float
	-mfloat-abi=hard
	-mcpu=cortex-m33
	-mfpu=fpv4-sp-d16
	-specs=nosys.specs
	-specs=nano.specs

	#-u _printf_float
)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} ${TARGET}
)
