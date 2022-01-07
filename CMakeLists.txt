cmake_minimum_required(VERSION 3.0)
project(MuMaNiu)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_definitions()

set(COMPILE_DEFINITIONS "$ENV{CXXFLAGS} -g -Wall -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wshadow -Wwrite-strings -march=native \
    -std=c++11 -rdynamic")

if(CMAKE_BUILD_BITS EQUAL 32)
  string(APPEND COMPILE_DEFINITIONS "-m32")
endif()

if (not CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

if (CMAKE_BUILD_TYPE EQUAL "Release")
    string(APPEND COMPILE_DEFINITIONS "-o2")
else()
    string(APPEND COMPILE_DEFINITIONS "-ggdb")
endif()
add_compile_definitions(${COMPILE_DEFINITIONS})

message(STATUS "build type:" ${CMAKE_BUILD_TYPE} "with options:" ${COMPILE_DEFINITIONS})

add_subdirectory(base)