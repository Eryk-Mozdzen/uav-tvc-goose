cmake_minimum_required(VERSION 3.16)

project(goose-gps)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/window.cpp"
	"src/interactive_map.cpp"

	"resources.qrc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"src"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Widgets
	Qt5::WebEngineWidgets
)
