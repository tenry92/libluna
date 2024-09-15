cmake_minimum_required(VERSION 3.21)
project(gfxtool)

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

find_library(LIBGFX NAMES gfx REQUIRED)
find_path(LIBGFX_INCLUDE_DIR NAMES libgfx/libgfx.h REQUIRED)

add_executable(gfxtool main.c bmfont.c encode.c loadpng.c surface.c utils.c)

# enable warnings and debug
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

target_link_libraries(gfxtool PUBLIC ${LIBGFX} ZLIB::ZLIB PNG::PNG)
target_include_directories(gfxtool PRIVATE ${LIBGFX_INCLUDE_DIR})

install(TARGETS gfxtool)
install(FILES $<TARGET_RUNTIME_DLLS:gfxtool> TYPE BIN)
install(TARGETS gfxtool
    RUNTIME ARCHIVE LIBRARY RUNTIME FRAMEWORK BUNDLE PUBLIC_HEADER RESOURCE)
install(TARGETS gfxtool
    COMPONENT gfxtool
    RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES $<TARGET_FILE_DIR:gfxtool>)

# copy DLLs on Windows: https://stackoverflow.com/a/75065206
