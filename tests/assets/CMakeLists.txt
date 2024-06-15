set(STATIC_FILES
  test.txt
  test32x32.png
  coin.png
  beat-layer1.opus beat-layer2.opus beat-layer3.opus
  textures/BricksDragfacedRunning008_COL_1K.png
  textures/BricksDragfacedRunning008_GLOSS_1K.png
  textures/BricksDragfacedRunning008_NRM_1K.png
  textures/BricksDragfacedRunning008_REFL_1K.png
  textures/ConcreteBlocksPavingSquareStack001_COL_1K.png
  textures/ConcreteBlocksPavingSquareStack001_GLOSS_1K.png
  textures/ConcreteBlocksPavingSquareStack001_NRM_1K.png
  textures/ConcreteBlocksPavingSquareStack001_REFL_1K.png
  caveman/character.png
  caveman/tileset_decoration.png
  caveman/tileset_solid.png
)

# compiled asset filenames
set(ASSET_FILES "")

set(ASSET_BUILD_FILES ${ASSET_FILES})
# file.txt => build/file.txt
list(TRANSFORM ASSET_BUILD_FILES PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_target(assets
  ALL
  DEPENDS ${ASSET_BUILD_FILES}
  COMMENT "Compiling assets"
)

set(ROMFS_FILES "")

if(NDS OR 3DS OR SWITCH)
  set(ASSETS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/romfs/assets")
else()
  set(ASSETS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()

foreach(static_file ${STATIC_FILES})
  set(input_filename "${CMAKE_CURRENT_SOURCE_DIR}/${static_file}")
  set(output_filename "${ASSETS_OUTPUT_DIR}/${static_file}")
  get_filename_component(output_dirname "${output_filename}" DIRECTORY)
  list(APPEND ROMFS_FILES "${output_filename}")
  add_custom_command(
    TARGET assets POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${input_filename}" "${output_filename}"
    DEPENDS "${input_filename}"
  )
endforeach()
