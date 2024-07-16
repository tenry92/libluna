include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

set(ASSETS_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(DATA_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/data")
set(ASSETS_OUTPUT_PATH "${DATA_OUTPUT_PATH}/assets")

luna_build_assets(
  "coin.png" "gfx" "f=rgba32" "s=32,32"
)

add_executable(anim_sprite_32bpp main.cpp)
add_dependencies(anim_sprite_32bpp luna gfx)
target_link_libraries(anim_sprite_32bpp PRIVATE luna gfx)
set_property(TARGET anim_sprite_32bpp PROPERTY ROMFS "${DATA_OUTPUT_PATH}")
luna_make_rom(anim_sprite_32bpp)
