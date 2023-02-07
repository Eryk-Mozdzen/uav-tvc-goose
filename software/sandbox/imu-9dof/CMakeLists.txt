cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "toolchain/arm-none-eabi.cmake")

project(imu-9dof)

enable_language(CXX C ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${TARGET}
	"../../third-party/cmsis_device_f4/Source/Templates/system_stm32f4xx.c"
	"../../third-party/cmsis_device_f4/Source/Templates/gcc/startup_stm32f411xe.s"

	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_uart.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_i2c.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_i2c_ex.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_gpio.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_rcc.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pwr.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_dma.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim_ex.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash_ex.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal_cortex.c"
	"../../third-party/stm32f4xx_hal_driver/Src/stm32f4xx_hal.c"

	"../../third-party/FreeRTOS-Kernel/tasks.c"
	"../../third-party/FreeRTOS-Kernel/queue.c"
	"../../third-party/FreeRTOS-Kernel/list.c"
	"../../third-party/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c"
	"../../third-party/FreeRTOS-Kernel/portable/MemMang/heap_4.c"

	"app/src/main.cpp"
	"app/src/imu9dof.cpp"
)

target_compile_definitions(${TARGET} PRIVATE
	STM32F411xE
	USE_HAL
)

target_include_directories(${TARGET} PRIVATE
	"../../third-party/cmsis_core/Include"
	"../../third-party/cmsis_device_f4/Include"
	"../../third-party/stm32f4xx_hal_driver/Inc"

	"../../third-party/FreeRTOS-Kernel/include"
	"../../third-party/FreeRTOS-Kernel/portable/GCC/ARM_CM4F"
	
	"app/inc"
	"app/inc/abstract"
)

target_compile_options(${TARGET} PRIVATE
	-O0
	-g
	
	-Wextra
	-Wpedantic
	-Wdouble-promotion
	
	-mcpu=cortex-m4
	-mfloat-abi=hard
	-mfpu=fpv4-sp-d16

	-specs=nosys.specs
)

target_link_options(${TARGET} PRIVATE
	-T ${CMAKE_SOURCE_DIR}/linker/STM32F411RETX_FLASH.ld

	-mthumb
	-mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard

	-specs=nosys.specs
)

target_link_libraries(${TARGET} m)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_SIZE} ${TARGET}
)
