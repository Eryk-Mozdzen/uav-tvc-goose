cmake_minimum_required(VERSION 3.16)

project(goose-station)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS	Widgets SerialPort WebEngineWidgets Gamepad REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/communication/usb.cpp"
	"src/communication/telnet.cpp"
	"src/communication/gamepad.cpp"
	"src/widgets/form.cpp"
	"src/window.cpp"
	"src/escape_codes.cpp"
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
	Qt5::Widgets
	Qt5::SerialPort
	Qt5::WebEngineWidgets
	Qt5::Gamepad
)
