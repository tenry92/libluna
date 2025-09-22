cmake_minimum_required(VERSION 3.12)
project(glad)

add_library(glad OBJECT src/glad.c)

target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
