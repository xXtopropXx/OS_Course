cmake_minimum_required(VERSION 3.3)
project(os)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES os_2/jmp_demo.cpp)
add_executable(os ${SOURCE_FILES})