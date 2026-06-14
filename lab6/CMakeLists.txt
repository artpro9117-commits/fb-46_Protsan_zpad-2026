cmake_minimum_required(VERSION 3.10)
project(Lab6CV)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(include ${OpenCV_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(Lab6CV ${SOURCES})
target_link_libraries(Lab6CV ${OpenCV_LIBS})