cmake_minimum_required(VERSION 3.16)

project(calibration)

set(CMAKE_AUTOMOC ON)

find_package(Eigen3 REQUIRED)
find_package(Qt6 COMPONENTS
	Widgets
	Network
	SerialPort
REQUIRED)

add_executable(${PROJECT_NAME}
	"../common/protocol/protocol.c"
	"../common/qt/Serial.cpp"
	"../common/qt/Telnet.cpp"

	"src/main.cpp"
	"src/Window.cpp"
	"src/magnetometer/Magnetometer.cpp"
	"src/magnetometer/Viewer.cpp"
	"src/accelerometer/Accelerometer.cpp"
	"src/gyroscope/Gyroscope.cpp"
	"src/servos/Servos.cpp"
	"src/load/Load.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"../common"
	"src"
	"src/magnetometer"
	"src/accelerometer"
	"src/gyroscope"
	"src/servos"
	"src/load"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-O3
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Eigen3::Eigen
	Qt6::Widgets
	Qt6::Network
	Qt6::SerialPort
)
