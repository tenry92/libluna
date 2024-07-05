cmake_minimum_required(VERSION 3.12)
project(gfxtool)

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

find_library(LIBGFX NAMES gfx REQUIRED)
find_path(LIBGFX_INCLUDE_DIR NAMES libgfx/libgfx.h REQUIRED)

add_executable(gfxtool main.c)

target_link_libraries(gfxtool PRIVATE ${LIBGFX} ZLIB::ZLIB PNG::PNG)
message("LIBGFX_INCLUDE_DIR: ${LIBGFX_INCLUDE_DIR}")
target_include_directories(gfxtool PRIVATE ${LIBGFX_INCLUDE_DIR})

install(TARGETS gfxtool)
