cmake_minimum_required(VERSION 3.25)

set(APPLICATION_NAME "cmyserver")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${APPLICATION_NAME} C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${APPLICATION_NAME} ${SOURCES})

target_compile_options(${APPLICATION_NAME} PRIVATE -Wall -Wextra)

target_include_directories(${APPLICATION_NAME} PRIVATE include)

