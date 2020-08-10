cmake_minimum_required(VERSION 3.17)
project(CppVideoTool)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )