cmake_minimum_required(VERSION 3.16)

project(calibration-mag)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Eigen3 REQUIRED)
find_package(Qt5 COMPONENTS
	Widgets
	SerialPort
	WebEngineWidgets
REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/usb.cpp"
	"src/window.cpp"
	"src/viewer.cpp"
	"src/telnet.cpp"
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
	Eigen3::Eigen
	Qt5::Widgets
	Qt5::SerialPort
	Qt5::WebEngineWidgets
)
