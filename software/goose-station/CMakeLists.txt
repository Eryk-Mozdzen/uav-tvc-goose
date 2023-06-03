cmake_minimum_required(VERSION 3.16)

project(goose-station)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Quick Widgets SerialPort Gamepad REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/communication/usb.cpp"
	"src/communication/telnet.cpp"
	"src/communication/gamepad.cpp"
	"src/widgets/form.cpp"
	"src/window.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"../transfer"
	"src/communication"
	"src/widgets"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Quick
	Qt5::Widgets
	Qt5::SerialPort
	Qt5::Gamepad
)
