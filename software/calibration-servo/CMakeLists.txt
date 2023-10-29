cmake_minimum_required(VERSION 3.16)

project(calibration-servo)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS
	Widgets
	SerialPort
	WebEngineWidgets
REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/window.cpp"
	"src/telnet.cpp"
	"src/usb.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"../transfer"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::SerialPort
	Qt5::WebEngineWidgets
)
