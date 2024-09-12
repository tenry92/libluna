include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

set(ASSETS_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(DATA_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/data")
set(ASSETS_OUTPUT_PATH "${DATA_OUTPUT_PATH}/assets")

luna_build_assets(
  "wall_32x32.png" "gfx" "f=rgba32"
  "pillar.obj" "static"
)

add_executable(room main.cpp)
add_dependencies(room luna gfx)
target_link_libraries(room PRIVATE luna gfx)
set_property(TARGET room PROPERTY ROMFS "${DATA_OUTPUT_PATH}")
luna_make_rom(room)
