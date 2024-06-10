cmake_minimum_required(VERSION 3.12)
project(glad)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(glad STATIC src/glad.c)

target_include_directories(glad PUBLIC ${INCLUDE_DIRS})

install(TARGETS glad
        EXPORT gladTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY ${INCLUDE_DIRS}/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)
