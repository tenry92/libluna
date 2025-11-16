include(${PROJECT_SOURCE_DIR}/cmake/LunaUtils.cmake)

add_custom_target(copy_assets_var_text ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/data/assets
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/assets/font.bmp
    ${CMAKE_CURRENT_BINARY_DIR}/data/assets/font.bmp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/assets/font.fnt
    ${CMAKE_CURRENT_BINARY_DIR}/data/assets/font.fnt
  COMMENT "Copying asset files to build directory"
)

add_executable(var_text main.cpp bmfont.h bmfont.c)

add_dependencies(var_text luna)
target_link_libraries(var_text PRIVATE luna)

add_dependencies(var_text copy_assets_var_text)
set_property(TARGET var_text PROPERTY ROMFS "${CMAKE_CURRENT_BINARY_DIR}/data")

luna_make_rom(var_text)
