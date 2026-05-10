cmake_minimum_required(VERSION 4.3.0)

set(APPLICATION_NAME "cmyserver")

project(${APPLICATION_NAME} C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${APPLICATION_NAME} ${SOURCES})

target_compile_options(${APPLICATION_NAME} PRIVATE -Wall -Wextra)

