cmake_minimum_required(VERSION 3.16)

project(station)

set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt6 COMPONENTS
	Widgets
	Network
	SerialPort
	PrintSupport
	OpenGL
REQUIRED)

pkg_check_modules(UDEV REQUIRED libudev)

add_executable(${PROJECT_NAME}
	"../common/math/utils.c"
	"../common/protocol/protocol.c"
	"../common/qt/Serial.cpp"
	"../common/qt/Network.cpp"
	"../common/qt/InterfaceWidget.cpp"
	"../third-party/QCustomPlot/qcustomplot/qcustomplot.cpp"

	"src/main.cpp"
	"src/Window.cpp"
	"src/Form.cpp"
	"src/LiveChart.cpp"
	"src/Gamepad.cpp"
	"src/Joystick.cpp"
	"src/Visualizer.cpp"
)

set_source_files_properties("../third-party/QCustomPlot/qcustomplot/qcustomplot.cpp" PROPERTIES COMPILE_FLAGS
	"-w"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	".."
	"../third-party"
	"src"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	QCUSTOMPLOT_USE_OPENGL
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-O3
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	udev
	Qt6::Widgets
	Qt6::Network
	Qt6::SerialPort
	Qt6::PrintSupport
	Qt6::OpenGL
)
