cmake_minimum_required(VERSION 3.16)

project(goose-visual)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Quick Widgets REQUIRED)
link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/communication/communication_base.cpp"
	"src/communication/communication_usb.cpp"
	"src/interface/display.cpp"
	"src/interface/escape_codes.cpp"
	"src/interface/writer.cpp"

	"qml/qml.qrc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"../transfer"
	"src/communication"
	"src/interface"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Quick
	Qt5::Widgets
	pthread
	serial
)
