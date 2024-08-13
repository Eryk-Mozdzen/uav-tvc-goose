cmake_minimum_required(VERSION 3.16)

project(station)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS
	Widgets
	Network
	SerialPort
	Charts
REQUIRED)

add_executable(${PROJECT_NAME}
	"../common/math/utils.c"
	"../common/protocol/protocol.c"
	"../common/qt/Serial.cpp"
	"../common/qt/Network.cpp"
	"../common/qt/AbstractInterface.cpp"

	"src/main.cpp"
	"src/Window.cpp"
	"src/Gamepad.cpp"
	"src/Form.cpp"
	"src/LiveChart.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	".."
	"src"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-O3
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Qt6::Widgets
	Qt6::Network
	Qt6::SerialPort
	Qt6::Charts
)
