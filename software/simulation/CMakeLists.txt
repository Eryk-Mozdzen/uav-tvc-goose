cmake_minimum_required(VERSION 3.16)

project(simulation)

set(CMAKE_AUTOMOC ON)

find_package(drake REQUIRED)
find_package(Qt5 COMPONENTS
	Widgets
	Network
	Gamepad
	Charts
REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/base/TrajectoryGenerator.cpp"
	"src/base/Controller.cpp"
	"src/generators/Square.cpp"
	"src/generators/Circle.cpp"
	"src/generators/Lemniscate.cpp"
	"src/generators/Manual.cpp"
	"src/controllers/Simple2.cpp"
	"src/controllers/Simple3.cpp"
	"src/plant/Plant.cpp"
	"src/interface/VisualClient.cpp"
	"src/interface/GraphXY.cpp"
	"src/interface/Chart.cpp"
	"src/interface/Gamepad.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"src/base"
	"src/generators"
	"src/controllers"
	"src/plant"
	"src/interface"
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
	Qt5::Widgets
	Qt5::Network
	Qt5::Gamepad
	Qt5::Charts
)
