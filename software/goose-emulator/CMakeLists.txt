cmake_minimum_required(VERSION 3.16)
project(goose-emulator)

set(TARGET ${PROJECT_NAME})

add_executable(${TARGET}
    main.cpp
	../transfer/transfer.cpp
)

target_link_libraries(${TARGET} serial)
target_include_directories(${TARGET} PRIVATE
	../transfer
)
