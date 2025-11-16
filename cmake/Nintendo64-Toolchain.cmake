if(NOT CMAKE_SYSTEM_NAME)
  set(CMAKE_SYSTEM_NAME Nintendo64)
endif()

set(NINTENDO_64 TRUE)

set(CMAKE_SYSTEM_PROCESSOR mips64)

set(LIBDRAGON_ROOT $ENV{LIBDRAGON_ROOT})

if(NOT LIBDRAGON_ROOT)
  set(LIBDRAGON_ROOT "/opt/libdragon")
endif()

set(CMAKE_SYSROOT "${LIBDRAGON_ROOT}/mips64-elf")
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_SYSROOT}")
endif()

find_program(CMAKE_ASM_COMPILER NAMES mips64-elf-gcc HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_C_COMPILER NAMES mips64-elf-gcc HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES mips64-elf-g++ HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_LINKER NAMES mips64-elf-ld HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_AR NAMES mips64-elf-ar HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_RANLIB NAMES mips64-elf-ranlib HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_STRIP NAMES mips64-elf-strip HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(N64_TOOL NAMES n64tool HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(CMAKE_OBJCOPY NAMES mips64-elf-objcopy HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(N64_ELFCOMPRESS NAMES n64elfcompress HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(N64_MKDFS NAMES mkdfs HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)
find_program(N64_SYM NAMES n64sym HINTS "${LIBDRAGON_ROOT}/bin" REQUIRED)

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
