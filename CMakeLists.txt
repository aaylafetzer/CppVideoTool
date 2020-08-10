cmake_minimum_required(VERSION 3.17)
project(CppVideoTool)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 20)

add_executable(CppVideoTool main.cpp)

target_link_libraries( CppVideoTool ${OpenCV_LIBS} )