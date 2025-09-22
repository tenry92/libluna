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
  if(CMAKE_SYSTEM_NAME STREQUAL "NintendoDS")
    if(target)
      set(romfs_arg NDSTOOL_NITROFS ${romfs})
      # ensure output folder exists as nx_create_nro complains during
      # configuration about missing folder
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
