set(ASSET_FILES
  "test.txt" "static"
  "test32x32.png" "static"
  "coin.png" "gfx" "s=32,32"
  "beat-layer1.opus" "static"
  "beat-layer2.opus" "static"
  "beat-layer3.opus" "static"
  "textures/BricksDragfacedRunning008_COL_1K.png" "gfx"
  "textures/BricksDragfacedRunning008_GLOSS_1K.png" "gfx"
  "textures/BricksDragfacedRunning008_NRM_1K.png" "gfx"
  "textures/BricksDragfacedRunning008_REFL_1K.png" "gfx"
  "textures/ConcreteBlocksPavingSquareStack001_COL_1K.png" "gfx"
  "textures/ConcreteBlocksPavingSquareStack001_GLOSS_1K.png" "gfx"
  "textures/ConcreteBlocksPavingSquareStack001_NRM_1K.png" "gfx"
  "textures/ConcreteBlocksPavingSquareStack001_REFL_1K.png" "gfx"
  "caveman/character.png" "gfx"
  "caveman/tileset_decoration.png" "gfx"
  "caveman/tileset_solid.png" "gfx"
)

set(ASSETS_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSETS_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/assets")

set(GFXTOOL "${LUNA_INSTALL_NATIVE_DIR}/bin/gfxtool")

function(make_target_name file)
  string(REPLACE "/" "_" target_name "${file}")
  set(${ARGV1} ${target_name} PARENT_SCOPE)
endfunction()

function(copy_static_file file)
  make_target_name("${file}" target_name)
  get_filename_component(name_rel_dir "${file}" DIRECTORY)
  set(output_file "${ASSETS_OUTPUT_PATH}/${name_rel_dir}/${file}")
  get_filename_component(name_dir "${output_file}" DIRECTORY)
  add_custom_command(
    OUTPUT "${output_file}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${name_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy "${ASSETS_INPUT_PATH}/${file}" "${output_file}"
    DEPENDS "${ASSETS_INPUT_PATH}/${file}" gfxtool-native
    COMMENT "Copying static asset file ${file}"
  )
  add_custom_target("copy_${target_name}" ALL DEPENDS "${output_file}")
endfunction()

function(convert_gfx file params)
  make_target_name("${file}" target_name)
  get_filename_component(name_we "${file}" NAME_WE)
  get_filename_component(name_rel_dir "${file}" DIRECTORY)
  set(output_file "${ASSETS_OUTPUT_PATH}/${name_rel_dir}/${name_we}.gfx")
  get_filename_component(name_dir "${output_file}" DIRECTORY)

  set(command "${GFXTOOL}" -e "${ASSETS_INPUT_PATH}/${file}" -o "${output_file}")
  foreach(param ${params})
    string(REPLACE "=" ";" param_pair "${param}")

    if("${param}" MATCHES "^.=.*")
      list(APPEND command "-${param_pair}")
    else()
      list(APPEND command "--${param_pair}")
    endif()
  endforeach()

  add_custom_command(
    OUTPUT "${output_file}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${name_dir}"
    COMMAND ${command}
    DEPENDS "${ASSETS_INPUT_PATH}/${file}" gfxtool-native
    COMMENT "Converting GFX file ${file} to ${output_file}; ${params}"
  )
  add_custom_target("convert_gfx_${target_name}" ALL DEPENDS "${output_file}")
endfunction()

list(LENGTH ASSET_FILES num_assets)
math(EXPR num_assets "${num_assets} - 1")
set(i 0)
while(i LESS num_assets)
  list(GET ASSET_FILES ${i} file)
  math(EXPR i "${i} + 1")
  list(GET ASSET_FILES ${i} type)

  if(type STREQUAL "static")
    copy_static_file("${file}")
    math(EXPR i "${i} + 1")
  elseif(type STREQUAL "gfx")
    set(params "")
    math(EXPR j "${i} + 1")
    while(j LESS num_assets)
      list(GET ASSET_FILES ${j} param)
      if("${param}" MATCHES "^[^=]+=[^=]+$")
        list(APPEND params "${param}")
        math(EXPR j "${j} + 1")
      else()
        break()
      endif()
    endwhile()
    convert_gfx("${file}" "${params}")
    set(i ${j})
  else()
    message(FATAL_ERROR "Unknown asset type: ${type}")
  endif()
endwhile()
