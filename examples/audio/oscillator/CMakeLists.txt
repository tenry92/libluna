include(${PROJECT_SOURCE_DIR}/cmake/LunaUtils.cmake)

add_executable(oscillator main.cpp)
add_dependencies(oscillator luna)
target_link_libraries(oscillator PRIVATE luna)
luna_make_rom(oscillator)
