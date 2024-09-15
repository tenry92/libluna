# Change special characters in filename so it can be used as a target name
# Example:
#   luna_luna_make_target_name("path/to/file.exe" target_name)
#   ("path_to_file.exe" is stored into ${target_name})
function(luna_make_target_name file)
  string(REPLACE "/" "_" target_name "${file}")
  set(${ARGV1} ${target_name} PARENT_SCOPE)
endfunction()

# Copy static file from ${ASSETS_INPUT_PATH} to ${ASSETS_OUTPUT_PATH}
# Example:
#   luna_assets_copy_static_file(file.txt)
function(luna_assets_copy_static_file file)
  # path/to/asset.txt -> path_to_asset.txt
  luna_make_target_name("${file}" target_name)

  # path/to/asset.txt -> path/to
  get_filename_component(name_rel_dir "${file}" DIRECTORY)

  set(output_file "${ASSETS_OUTPUT_PATH}/${name_rel_dir}/${file}")

  # ${ASSETS_OUTPUT_PATH}/path/to/asset.txt -> ${ASSETS_OUTPUT_PATH}/path/to
  get_filename_component(name_dir "${output_file}" DIRECTORY)

  add_custom_command(
    OUTPUT "${output_file}"
    # ensure parent directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory "${name_dir}"
    # actually copy the file
    COMMAND ${CMAKE_COMMAND} -E copy "${ASSETS_INPUT_PATH}/${file}" "${output_file}"
    # depends on the source file
    DEPENDS "${ASSETS_INPUT_PATH}/${file}"
    COMMENT "Copying static asset file ${file}"
  )
  # add target file as a custom command that is executed by default
  add_custom_target("assets_copy_${target_name}" ALL DEPENDS "${output_file}")
endfunction()

# Convert image file from ${ASSETS_INPUT_PATH} to a GFX file to ${ASSETS_OUTPUT_PATH}
# file: the input file to convert
# type: gfx, font
# params: additional conversion parameter
# Example:
#     luna_assets_convert_gfx(sprite.png gfx f=rgba16;s=32,32)
function(luna_assets_convert_gfx file type params)
  if(NOT DEFINED GFXTOOL OR GFXTOOL STREQUAL "")
    message(FATAL_ERROR "GFXTOOL variable is not set or is empty. Please set this variable before running CMake.")
  endif()

  # path/to/asset.png -> path_to_asset.png
  luna_make_target_name("${file}" target_name)

  # path/to/asset.png -> asset
  get_filename_component(name_we "${file}" NAME_WE)

  # path/to/asset.png -> path/to
  get_filename_component(name_rel_dir "${file}" DIRECTORY)

  # output_file = ${ASSETS_OUTPUT_PATH}/path/to/asset.gfx
  set(output_file "${ASSETS_OUTPUT_PATH}/${name_rel_dir}/${name_we}.gfx")
  string(REPLACE "//" "/" output_file "${output_file}")

  # ${ASSETS_OUTPUT_PATH}/path/to/asset.txt -> ${ASSETS_OUTPUT_PATH}/path/to
  get_filename_component(name_dir "${output_file}" DIRECTORY)

  set(command "${GFXTOOL}" encode)
  foreach(param ${params})
    # f=rgba32 -> f;rgba32
    string(REPLACE "=" ";" param_pair "${param}")

    if("${param}" MATCHES "^.=.*")
      # f=rgba32
      # "${GFXTOOL};encode" -> "${GFXTOOL};encode;-f;rgba32"
      list(APPEND command "-${param_pair}")
    else()
      # format=rgba32
      # "${GFXTOOL};encode" -> "${GFXTOOL};encode;--format;rgba32"
      list(APPEND command "--${param_pair}")
    endif()
  endforeach()

  if(type STREQUAL "font")
    list(APPEND command --font "${ASSETS_INPUT_PATH}/${file}" -o "${output_file}")
  else()
    list(APPEND command -i "${ASSETS_INPUT_PATH}/${file}" -o "${output_file}")
  endif()

  add_custom_command(
    OUTPUT "${output_file}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${name_dir}"
    COMMAND ${command}
    DEPENDS "${ASSETS_INPUT_PATH}/${file}" gfxtool-native
    COMMENT "Converting GFX file ${file} to ${output_file}; ${params}"
  )
  add_custom_target("assets_convert_gfx_${target_name}" ALL DEPENDS "${output_file}")
endfunction()

# Build assets files.
# ${asset_files} is a list of file-type pairs, optionally followed by options.
# Options related to the current asset are recognized by the presence of the
# equal sign "=".
# Example:
#   luna_build_assets(asset_files
#     "text.txt"   "static"                      # copy
#     "sprite.png" "gfx"    "s=32,32" "f=rgba16" # convert png to gfx
#   )
function(luna_build_assets)
  set(asset_files ${ARGV})
  message(STATUS "Collecting assets")
  list(LENGTH asset_files num_assets)
  math(EXPR num_assets "${num_assets} - 1")
  set(i 0)
  while(i LESS num_assets)
    # e.g. asset.txt
    list(GET asset_files ${i} file)
    math(EXPR i "${i} + 1")

    # e.g. static
    list(GET asset_files ${i} type)
    math(EXPR i "${i} + 1")

    if(type STREQUAL "static")
      luna_assets_copy_static_file("${file}")
    elseif(type STREQUAL "gfx" OR type STREQUAL "font")
      set(params "")
      set(j ${i})
      while(j LESS_EQUAL num_assets)
        list(GET asset_files ${j} param)
        if("${param}" MATCHES "^[^=]+=[^=]+$")
          list(APPEND params "${param}")
          math(EXPR j "${j} + 1")
        else()
          break()
        endif()
      endwhile()
      luna_assets_convert_gfx("${file}" ${type} "${params}")
      set(i ${j})
    else()
      message(FATAL_ERROR "Unknown asset type: ${type}")
    endif()
  endwhile()
endfunction()

# If supported by the current target system, instruct CMake to generate a ROM
# file.
# The following properties on the target may be respected:
#
#   SWITCH_ROM_ICON
#   3DS_ROM_ICON
#   NDS_ROM_ICON
#   ROMFS
function(luna_make_rom target)
  get_property(romfs TARGET ${target} PROPERTY ROMFS)
  if(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    if(target)
      set(romfs_arg ROMFS ${romfs})
      # ensure output folder exists as nx_create_nro complains during
      # configuration about missing folder
      file(MAKE_DIRECTORY "${romfs}")
    endif()
    nx_create_nro(${target} ${romfs_arg})
  endif()
  if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
    if(target)
      set(romfs_arg DFS ${romfs})
    endif()
    n64_create_rom(${target} ${romfs_arg})
  endif()
endfunction()
