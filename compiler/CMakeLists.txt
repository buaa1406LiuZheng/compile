cmake_minimum_required(VERSION 3.6)
project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c lexical.h syntax.h table.h lexical.c error.h error.c syntax.c table.c mips.h mips.c)
add_executable(compiler ${SOURCE_FILES})