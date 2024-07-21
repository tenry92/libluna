include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

set(ASSETS_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(DATA_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/data")
set(ASSETS_OUTPUT_PATH "${DATA_OUTPUT_PATH}/assets")

luna_build_assets(
  "color-test_16bit.png" "gfx" "f=rgba16"
)

add_executable(sprite_16bpp main.cpp)
add_dependencies(sprite_16bpp luna gfx)
target_link_libraries(sprite_16bpp PRIVATE luna gfx)
set_property(TARGET sprite_16bpp PROPERTY ROMFS "${DATA_OUTPUT_PATH}")
luna_make_rom(sprite_16bpp)
