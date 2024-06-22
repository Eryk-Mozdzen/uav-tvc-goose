cmake_minimum_required(VERSION 3.16)

project(simulation)

find_package(drake REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/base/TrajectoryGenerator.cpp"
	"src/base/Controller.cpp"
	"src/generators/Circle.cpp"
	"src/generators/Lemniscate.cpp"
	"src/controllers/Simple3.cpp"
	"src/plant/Plant.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"src/base"
	"src/generators"
	"src/controllers"
	"src/plant"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-std=c++20
	-O3
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	drake::drake
)
