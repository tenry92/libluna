cmake_minimum_required(VERSION 3.12)
project(libgfx)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
add_library(gfx STATIC libgfx/libgfx.c)
target_compile_features(gfx PUBLIC c_std_11)

if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/Zi /Od")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Zi /DNDEBUG")
  set(CMAKE_C_FLAGS_MINSIZEREL "/O1 /DNDEBUG")
else()
  set(CMAKE_C_FLAGS_DEBUG "-g")
  set(CMAKE_C_FLAGS_RELEASE "-O3")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
  set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
endif()

target_include_directories(gfx PUBLIC ${INCLUDE_DIRS})

install(TARGETS gfx)
install(FILES libgfx/libgfx.h DESTINATION include/libgfx)
