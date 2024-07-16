include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

add_executable(bgcolor main.cpp)
add_dependencies(bgcolor luna)
target_link_libraries(bgcolor PRIVATE luna)
luna_make_rom(bgcolor)
