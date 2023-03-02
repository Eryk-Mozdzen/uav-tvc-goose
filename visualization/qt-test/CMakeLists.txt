cmake_minimum_required(VERSION 3.16)

project(qt-test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/myclass.cpp"
	"qml/qml.qrc"
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
	Qt5::Core
	Qt5::Quick
)
