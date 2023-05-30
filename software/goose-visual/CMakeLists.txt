cmake_minimum_required(VERSION 3.16)

project(goose-visual)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Quick Widgets SerialPort REQUIRED)

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc_doxygen ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen"
)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"../transfer/transfer.cpp"
	"src/communication/usb.cpp"
	"src/communication/telnet.cpp"
	"src/cli-interface/escape_codes.cpp"
	"src/cli-interface/printer_writer.cpp"
	"src/gui-interface/logger.cpp"
	"src/gui-interface/battery.cpp"
	"src/gui-interface/altimeter.cpp"
	"src/gui-interface/scene.cpp"
	"src/gui-interface/actuators.cpp"

	"qml/resources.qrc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
	"../transfer"
	"src/communication"
	"src/cli-interface"
	"src/gui-interface"
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
