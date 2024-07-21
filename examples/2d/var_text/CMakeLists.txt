include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

set(ASSETS_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(DATA_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/data")
set(ASSETS_OUTPUT_PATH "${DATA_OUTPUT_PATH}/assets")

luna_build_assets(
  "font.txt" "font"
)

add_executable(var_text main.cpp)
add_dependencies(var_text luna gfx)
target_link_libraries(var_text PRIVATE luna gfx)
set_property(TARGET var_text PROPERTY ROMFS "${DATA_OUTPUT_PATH}")
luna_make_rom(var_text)
