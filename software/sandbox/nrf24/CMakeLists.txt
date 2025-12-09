cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "toolchain.cmake")

project(nrf24)

enable_language(ASM C CXX)

set(TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${TARGET}
    "cubemx/startup_stm32u083xx.s"
    "cubemx/Core/Src/main.c"
    "cubemx/Core/Src/stm32u0xx_hal_msp.c"
    "cubemx/Core/Src/stm32u0xx_hal_timebase_tim.c"
    "cubemx/Core/Src/stm32u0xx_it.c"
    "cubemx/Core/Src/syscalls.c"
    "cubemx/Core/Src/sysmem.c"
    "cubemx/Core/Src/system_stm32u0xx.c"

    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_crc_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rng.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_adc.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_usart.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_usart_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_i2c_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_spi.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_smartcard_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rtc_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_tim_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pcd.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pwr_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_opamp_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_flash_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_uart.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rcc.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dac.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_crc.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_uart_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pwr.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_opamp.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_exti.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pcd_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dma.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rcc_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_gpio.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_cryp.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_wwdg.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_spi_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_cortex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_tsc.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_i2c.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_iwdg.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_tim.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_smartcard.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rtc.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_lcd.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_flash.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_lptim.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_cryp_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_comp.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rng_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dac_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_adc_ex.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_irda.c"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dma_ex.c"

    "FreeRTOS-Kernel/croutine.c"
    "FreeRTOS-Kernel/event_groups.c"
    "FreeRTOS-Kernel/list.c"
    "FreeRTOS-Kernel/queue.c"
    "FreeRTOS-Kernel/stream_buffer.c"
    "FreeRTOS-Kernel/tasks.c"
	"FreeRTOS-Kernel/timers.c"
    "FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c"
    "FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c"
    "FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c"

	"src/system/main.c"
    "src/system/freertos.c"
    "src/topic/Topics.cpp"
    "src/Blink.cpp"
    "src/Button.cpp"
    "src/Led.cpp"
    "src/Serial.cpp"
)

target_include_directories(${TARGET} PRIVATE
    "cubemx/Core/Inc"
    "cubemx/Drivers/CMSIS/Include"
    "cubemx/Drivers/CMSIS/Device/ST/STM32U0xx/Include"
    "cubemx/Drivers/STM32U0xx_HAL_Driver/Inc"
    "FreeRTOS-Kernel/include"
    "FreeRTOS-Kernel/portable/GCC/ARM_CM0"
	"src"
    "src/system"
)

target_compile_definitions(${TARGET} PRIVATE
    USE_HAL_DRIVER
    STM32U083xx
)

target_compile_options(${TARGET} PRIVATE
	$<$<COMPILE_LANGUAGE:ASM>:
        -x assembler-with-cpp

        -mthumb
        -mcpu=cortex-m0
	>

	$<$<COMPILE_LANGUAGE:C>:
		-Os
		-std=c23
		-Wall
        -Wextra
        -Wpedantic

        -Wno-unused-parameter

		-ffunction-sections
		-fdata-sections

        -mthumb
		-mcpu=cortex-m0
	>

    $<$<COMPILE_LANGUAGE:CXX>:
		-Os
		-std=c++17
		-Wall
        -Wextra
        -Wpedantic

        -Wno-unused-parameter

		-ffunction-sections
		-fdata-sections

        -mthumb
		-mcpu=cortex-m0
	>
)

target_link_options(${TARGET} PRIVATE
	-T ${CMAKE_SOURCE_DIR}/cubemx/STM32U083xx_FLASH.ld
	-Wl,-Map=${CMAKE_PROJECT_NAME}.map
	-Wl,--gc-sections
	-Wl,-cref
    -Wl,--start-group -lc -lm -Wl,--end-group

    -mthumb
	-mcpu=cortex-m0
)

target_link_libraries(${TARGET}
	m
)

add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -j .text -D ${TARGET} > ${CMAKE_PROJECT_NAME}.dump
	COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} ${TARGET}
)
