cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "arm-none-eabi.cmake")

project(goose-fc)

enable_language(C ASM)

set(TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${TARGET}
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_ll_usb.c"
	"../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_i2c.c"
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

	"startup/system_stm32f4xx.c"
	"startup/startup_stm32f411xe.s"

	"FreeRTOS/ARM_CM4F/port.c"
	"FreeRTOS/MemMang/heap_4.c"

	"HAL/stm32f4xx_hal_it.c"
	"HAL/stm32f4xx_hal_msp.c"
	"HAL/stm32f4xx_hal_timebase.c"
	"HAL/hal_callbacks.c"

	"USB/usbd_cdc_if.c"
	"USB/usbd_conf.c"
	"USB/usbd_desc.c"
	"USB/usb_device.c"

	"src/main.c"
	"src/sensors/sensors.c"
	"src/sensors/sensor_bus.c"
	"src/sensors/bmp280.c"
	"src/communication/logger.c"
	"src/communication/communication.c"
	"src/estimator/state_estimator.c"
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
	
	"FreeRTOS/ARM_CM4F"
	"FreeRTOS/MemMang"
	"FreeRTOS"
	"HAL"
	"USB"

	"src"
	"src/sensors"
	"src/communication"
	"src/estimator"
)

target_compile_options(${TARGET} PRIVATE
	-O0
	-g3

	-Wextra
	-Wpedantic
	-Wdouble-promotion

	-ffunction-sections
	-fdata-sections
	-fstack-usage
	
	-mcpu=cortex-m4
	-mfloat-abi=hard
	-mfpu=fpv4-sp-d16

	-specs=nosys.specs
)

target_link_options(${TARGET} PRIVATE
	-T ${CMAKE_SOURCE_DIR}/STM32F411RETX_FLASH.ld
	-Wl,-Map=${CMAKE_PROJECT_NAME}.map
	-Wl,--gc-sections

	-mthumb
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard

	-specs=nosys.specs
)

target_link_libraries(${TARGET} 
	m
)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} ${TARGET}
)
