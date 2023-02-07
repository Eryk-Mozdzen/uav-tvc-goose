cmake_minimum_required(VERSION 3.16)

project(ricatti)

add_compile_options(
	-O0
	-Wall 
	-pedantic 
	-std=c++2a 
	#-fconstexpr-ops-limit=100000000
	#-ftime-report
)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	m
)
