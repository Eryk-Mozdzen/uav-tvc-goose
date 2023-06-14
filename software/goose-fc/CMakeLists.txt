cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "arm-none-eabi.cmake")

project(goose-fc)

enable_language(ASM C CXX)

set(TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${TARGET}
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_ll_usb.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_i2c.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_uart.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_can.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pcd.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pcd_ex.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_adc.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_adc_ex.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_exti.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_gpio.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_rcc.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pwr.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_dma.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim_ex.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash_ex.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_cortex.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal.c"

	"../third-party/FreeRTOS-Kernel/tasks.c"
	"../third-party/FreeRTOS-Kernel/queue.c"
	"../third-party/FreeRTOS-Kernel/timers.c"
	"../third-party/FreeRTOS-Kernel/list.c"

	"../third-party/stm32_mw_usb_device/Core/Src/usbd_core.c"
	"../third-party/stm32_mw_usb_device/Core/Src/usbd_ctlreq.c"
	"../third-party/stm32_mw_usb_device/Core/Src/usbd_ioreq.c"
	"../third-party/stm32_mw_usb_device/Class/CDC/Src/usbd_cdc.c"

	"../third-party/VL53L0X/core/src/vl53l0x_api_calibration.c"
	"../third-party/VL53L0X/core/src/vl53l0x_api_core.c"
	"../third-party/VL53L0X/core/src/vl53l0x_api_ranging.c"
	"../third-party/VL53L0X/core/src/vl53l0x_api_strings.c"
	"../third-party/VL53L0X/core/src/vl53l0x_api.c"
	"../third-party/VL53L0X/platform/src/vl53l0x_platform_log.c"
	"../third-party/VL53L0X/platform/src/vl53l0x_platform.c"

	"../transfer/transfer.cpp"

	"startup/system_stm32f4xx.c"
	"startup/startup_stm32f411xe.s"

	"FreeRTOS/ARM_CM4F/port.c"
	"FreeRTOS/freertos_exceptions.c"
	"FreeRTOS/freertos_runtime_stats.c"

	"HAL/stm32f4xx_hal_it.c"
	"HAL/stm32f4xx_hal_msp.c"
	"HAL/stm32f4xx_hal_timebase.c"
	"HAL/hal_callbacks.c"
	"HAL/hal_exceptions.c"

	"USB/usbd_cdc_if.cpp"
	"USB/usbd_conf.c"
	"USB/usbd_desc.c"
	"USB/usb_device.c"

	"src/main.cpp"
	"src/blink.cpp"
	"src/stats.cpp"
	"src/sensors/sensor_bus.cpp"
	"src/sensors/bmp280.cpp"
	"src/sensors/mpu6050.cpp"
	"src/sensors/hmc5883l.cpp"
	"src/sensors/vl53l0x.cpp"
	"src/sensors/power_stats.cpp"
	"src/communication/logger.cpp"
	"src/communication/wire_transmitter.cpp"
	"src/communication/wire_receiver.cpp"
	"src/communication/communication_bus.cpp"
	"src/communication/wireless_transmitter.cpp"
	"src/communication/wireless_receiver.cpp"
	"src/estimator/attitude_estimator.cpp"
	"src/estimator/altitude_estimator.cpp"
	"src/estimator/battery_estimator.cpp"
	"src/estimator/state_estimator.cpp"
	"src/utils/quaternion.cpp"
	"src/utils/message.cpp"
	"src/transport/transport.cpp"
	"src/control/control.cpp"
	"src/control/states.cpp"
	"src/control/events.cpp"
	"src/control/actuators.cpp"
	"src/control/controller.cpp"
)

target_compile_definitions(${TARGET} PRIVATE
	STM32F411xE
	USE_HAL
)

target_include_directories(${TARGET} PRIVATE
	"../third-party/cmsis_core/Include"
	"../third-party/cmsis_device_f4/Include"
	"../third-party/stm32f4xx_hal_driver/Inc"

	"../third-party/FreeRTOS-Kernel/include"

	"../third-party/stm32_mw_usb_device/Core/Inc"
	"../third-party/stm32_mw_usb_device/Class/CDC/Inc"

	"../third-party/VL53L0X/core/inc"
	"../third-party/VL53L0X/platform/inc"

	"../third-party/FreeRTOScpp"

	"../third-party/state-machine-cpp/sm"

	"../transfer"

	"FreeRTOS/ARM_CM4F"
	"FreeRTOS/MemMang"
	"FreeRTOS"
	"HAL"
	"USB"

	"src"
	"src/sensors"
	"src/communication"
	"src/estimator"
	"src/utils"
	"src/transport"
)

target_compile_options(${TARGET} PRIVATE
	$<$<COMPILE_LANGUAGE:ASM>:
		-Os -g

		-mthumb
		-mhard-float
		-mcpu=cortex-m4
		-mfpu=fpv4-sp-d16
		-specs=nano.specs

		-x assembler-with-cpp
	>

	$<$<COMPILE_LANGUAGE:C>:
		-Os -g
		-std=c17

		-Wall
		-Wdouble-promotion

		-ffunction-sections
		-fdata-sections
		-fstack-usage

		-mthumb
		-mhard-float
		-mcpu=cortex-m4
		-mfpu=fpv4-sp-d16
		-specs=nano.specs
	>

	$<$<COMPILE_LANGUAGE:CXX>:
		-Os -g
		-std=c++17

		-Wall
		-Wextra
		-Wpedantic
		-Wdouble-promotion
		-Werror=return-type

		-ffunction-sections
		-fdata-sections
		-fstack-usage
		-fno-threadsafe-statics
		-fno-common
		-fno-exceptions
		-fno-rtti

		-mthumb
		-mhard-float
		-mcpu=cortex-m4
		-mfloat-abi=hard
		-mfpu=fpv4-sp-d16
		-specs=nosys.specs
		-specs=nano.specs
	>
)

target_link_options(${TARGET} PRIVATE
	-T ${CMAKE_SOURCE_DIR}/STM32F411RETX_FLASH.ld
	-Wl,-Map=${CMAKE_PROJECT_NAME}.map
	-Wl,--gc-sections
	-Wl,-cref
	-Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

	-mthumb
	-mhard-float
	-mfloat-abi=hard
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-specs=nosys.specs
	-specs=nano.specs

	-static
	-u _printf_float
)

target_link_libraries(${TARGET}
	m
)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} ${TARGET}
)
