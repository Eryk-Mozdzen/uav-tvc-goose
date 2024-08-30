cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE arm-none-eabi.cmake)

project(cmsisdsp)

include_directories(
    CMSIS_5/CMSIS/Core/Include
    CMSIS-DSP/Include
    CMSIS-DSP/PrivateInclude
)

file(GLOB_RECURSE SOURCES
    CMSIS-DSP/Source/*.c
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ARM_MATH_CM33
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -Ofast
)

target_link_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -Ofast
)
