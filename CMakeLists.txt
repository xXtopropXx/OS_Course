cmake_minimum_required(VERSION 3.3)
project(os)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES os_2/uthreads.cpp os_2/queueFacade.h os_2/threads_library.h os_2/lib_exceptions.h os_2/thread.h)
add_executable(os ${SOURCE_FILES})