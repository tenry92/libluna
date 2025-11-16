include(${PROJECT_SOURCE_DIR}/cmake/LunaUtils.cmake)

add_custom_target(copy_assets_anim_sprite_32bpp ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/data/assets
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/assets/coin_16bpp.bmp
    ${CMAKE_CURRENT_BINARY_DIR}/data/assets/coin_16bpp.bmp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/assets/coin_24bpp.bmp
    ${CMAKE_CURRENT_BINARY_DIR}/data/assets/coin_24bpp.bmp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/assets/coin_32bpp.bmp
    ${CMAKE_CURRENT_BINARY_DIR}/data/assets/coin_32bpp.bmp
  COMMENT "Copying asset files to build directory"
)

add_executable(anim_sprite_32bpp main.cpp)

add_dependencies(anim_sprite_32bpp luna)
target_link_libraries(anim_sprite_32bpp PRIVATE luna)

add_dependencies(anim_sprite_32bpp copy_assets_anim_sprite_32bpp)
set_property(TARGET anim_sprite_32bpp PROPERTY ROMFS "${CMAKE_CURRENT_BINARY_DIR}/data")

luna_make_rom(anim_sprite_32bpp)
