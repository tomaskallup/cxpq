cmake_minimum_required(VERSION 3.10)

project(cxpq)

set(CMAKE_DEBUG_POSTFIX _debug)

set(CMAKE_C_FLAGS_DEBUG
  "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG
    "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

file(GLOB Parsers parsers/*.c)

add_executable(cxpq cxpq.c string.c util.c xml-parser.c ${Parsers})

set_target_properties(cxpq PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
