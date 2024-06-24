cmake_minimum_required(VERSION 3.12)
project(libgfx)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
add_library(libgfx STATIC libgfx/libgfx.c)

target_include_directories(libgfx PUBLIC ${INCLUDE_DIRS})


find_package(PNG REQUIRED)

add_executable(gfxtool
  libgfx/gfxtool.c
)

add_dependencies(gfxtool libgfx)
target_link_libraries(gfxtool PRIVATE libgfx PNG::PNG)
