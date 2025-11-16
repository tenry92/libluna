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
  if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo3DS")
    if(target)
      set(CTR_3DSXTOOL_ROMFS ${romfs})
      file(MAKE_DIRECTORY "${romfs}")
    endif()
    ctr_create_3dsx(${target})
  endif()
  if(CMAKE_SYSTEM_NAME STREQUAL "NintendoDS")
    if(target)
      set(romfs_arg NDSTOOL_NITROFS ${romfs})
      file(MAKE_DIRECTORY "${romfs}")
    endif()
    nds_create_rom(${target} ${romfs_arg})
  endif()
  if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
    if(target)
      set(romfs_arg DFS ${romfs})
    endif()
    n64_create_rom(${target} ${romfs_arg})
  endif()
endfunction()

function(n64_create_rom target)
  set(TITLE_ARG "")

  cmake_parse_arguments(ARG "" "TITLE;DFS" "" ${ARGN})

  if(ARG_TITLE)
    set(TITLE_ARG "--title;\"${ARG_TITLE}\"")
  endif()

  set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${target}")
  set(SYM_PATH "${CMAKE_CURRENT_BINARY_DIR}/${target}.sym")
  set(ELF_STRIPPED_PATH "${ELF_PATH}.stripped")
  set(ELF_STRIPPED_COMPRESSED_PATH "${ELF_STRIPPED_PATH}.compressed")
  set(DFS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.dfs")
  set(N64_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.z64")

  if(ARG_DFS)
    add_custom_command(
      TARGET ${target}
      COMMAND ${N64_MKDFS} ${DFS_OUTPUT} ${ARG_DFS}
      COMMENT "Creating DFS file ${DFS_OUTPUT}"
    )
    set(DFS_ARG "--align;16;${DFS_OUTPUT}")
  endif()

  add_custom_command(
    OUTPUT ${N64_OUTPUT}
    DEPENDS ${target}
    COMMAND ${N64_SYM} ${ELF_PATH} ${SYM_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${ELF_PATH} ${ELF_STRIPPED_PATH}
    COMMAND ${CMAKE_STRIP} -s ${ELF_STRIPPED_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${ELF_STRIPPED_PATH} ${ELF_STRIPPED_COMPRESSED_PATH}
    COMMAND ${N64_ELFCOMPRESS} -c 1 ${ELF_STRIPPED_COMPRESSED_PATH}
    COMMAND ${N64_TOOL} ${TITLE_ARG} --toc --output ${N64_OUTPUT} --align 256 ${ELF_STRIPPED_COMPRESSED_PATH} --align 8 ${SYM_PATH} ${DFS_ARG}
    COMMAND ${CMAKE_COMMAND} -E remove ${ELF_STRIPPED_PATH} ${ELF_STRIPPED_COMPRESSED_PATH}
    COMMENT "Creating N64 ROM ${N64_OUTPUT}"
    VERBATIM
  )

  add_custom_target(${target}_n64 ALL DEPENDS ${N64_OUTPUT})
endfunction()
