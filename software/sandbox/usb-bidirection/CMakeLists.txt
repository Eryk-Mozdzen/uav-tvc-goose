cmake_minimum_required(VERSION 3.16)

project(usb-bidirectional)

add_compile_options(
	-O0
	-Wall
	-Wextra
	-pedantic
	-std=c++2a
)

link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME}
	"main.cpp"
	"../../transfer/transfer.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"../../transfer"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	m
	serial
	pthread
)
