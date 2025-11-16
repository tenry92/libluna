include(${PROJECT_SOURCE_DIR}/cmake/LunaUtils.cmake)

add_executable(sprite main.cpp)
add_dependencies(sprite luna)
target_link_libraries(sprite PRIVATE luna)
luna_make_rom(sprite)
