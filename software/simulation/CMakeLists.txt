cmake_minimum_required(VERSION 3.16)

project(simulation)

add_subdirectory("mujoco")

find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"mujoco/include"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-std=c++20
	-O3
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	mujoco::mujoco
	glfw
)
