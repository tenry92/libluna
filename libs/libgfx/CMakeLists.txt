cmake_minimum_required(VERSION 3.12)
project(libgfx)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
add_library(gfx STATIC libgfx/libgfx.c)
target_compile_features(gfx PUBLIC c_std_11)

target_include_directories(gfx PUBLIC ${INCLUDE_DIRS})

install(TARGETS gfx)
install(FILES libgfx/libgfx.h DESTINATION include/libgfx)
