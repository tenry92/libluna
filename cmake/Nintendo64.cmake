if(NOT CMAKE_SYSTEM_NAME)
  set(CMAKE_SYSTEM_NAME Nintendo64)
endif()

set(NINTENDO_64 TRUE)

set(CMAKE_SYSTEM_PROCESSOR mips64)

set(LIBDRAGON_ROOT /opt/libdragon)
set(CMAKE_SYSROOT "${LIBDRAGON_ROOT}/mips64-elf")
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_SYSROOT}")
endif()

find_program(CMAKE_ASM_COMPILER NAMES mips64-elf-gcc HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_C_COMPILER NAMES mips64-elf-gcc HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_CXX_COMPILER NAMES mips64-elf-g++ HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_LINKER NAMES mips64-elf-ld HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_AR NAMES mips64-elf-ar HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_RANLIB NAMES mips64-elf-ranlib HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_STRIP NAMES mips64-elf-strip HINTS "${LIBDRAGON_ROOT}/bin")
find_program(N64_TOOL NAMES n64tool HINTS "${LIBDRAGON_ROOT}/bin")
find_program(CMAKE_OBJCOPY NAMES mips64-elf-objcopy HINTS "${LIBDRAGON_ROOT}/bin")
find_program(N64_ELFCOMPRESS NAMES n64elfcompress HINTS "${LIBDRAGON_ROOT}/bin")
find_program(N64_MKDFS NAMES mkdfs HINTS "${LIBDRAGON_ROOT}/bin")
find_program(N64_SYM NAMES n64sym HINTS "${LIBDRAGON_ROOT}/bin")

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_C_COMPILE_FEATURES c_std_90 c_std_99 c_std_11 c_std_17 c_std_23)
set(CMAKE_CXX_COMPILE_FEATURES cxx_std_11 cxx_std_14 cxx_std_17 cxx_std_20 cxx_std_23)

find_path(LIBDRAGON_INCLUDE_DIR NAMES libdragon.h HINTS "${LIBDRAGON_ROOT}/mips64-elf/include")
set(LIBDRAGON_LIB "${LIBDRAGON_ROOT}/mips64-elf/lib/libdragon.a")

add_compile_definitions(N64)
add_compile_options(-march=vr4300 -mtune=vr4300 -I${LIBDRAGON_INCLUDE_DIR})
add_compile_options(-falign-functions=32)
add_compile_options(-ffunction-sections -fdata-sections -ffile-prefix-map="${CMAKE_CURRENT_DIR}"=)
add_compile_options(-ffast-math -ftrapping-math -fno-associative-math)
add_link_options(-g -Tn64.ld -Wl,--gc-sections,--wrap,__do_global_ctors)
link_libraries(dragon c m dragonsys)

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
