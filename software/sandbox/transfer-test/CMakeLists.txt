cmake_minimum_required(VERSION 3.16)

project(transfer-test)

add_subdirectory("../../transfer" "transfer")

add_compile_options(
	-O0
	-Wall 
	-Wextra
	-pedantic 
	-std=c++2a 
)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	m
	transfer
)
