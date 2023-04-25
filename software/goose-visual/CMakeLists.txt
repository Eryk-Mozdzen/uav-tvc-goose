cmake_minimum_required(VERSION 3.16)

project(goose-visual)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Doxygen)

find_package(Qt5 COMPONENTS Quick Widgets SerialPort REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/communication/usb.cpp"
	"src/communication/telnet.cpp"
	"src/interface/escape_codes.cpp"
	"src/interface/printer_writer.cpp"

	"qml/resources.qrc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"../transfer"
	"src/communication"
	"src/interface"
)

add_custom_target(doc_doxygen ALL
	COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
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
)
