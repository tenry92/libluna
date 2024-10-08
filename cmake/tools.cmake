# these are always built for the host platform

if(MSVC)
  set(EXT_PROJ_TOOLCHAIN CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

ExternalProject_Add(
  libgfx-native
  PREFIX "${LUNA_BUILD_NATIVE_DIR}"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_PREFIX_PATH=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  ${EXT_PROJ_TOOLCHAIN}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libgfx
  BUILD_ALWAYS ON
)

ExternalProject_Add(
  gfxtool-native
  PREFIX "${LUNA_BUILD_NATIVE_DIR}"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_PREFIX_PATH=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  ${EXT_PROJ_TOOLCHAIN}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools/gfxtool
  BUILD_ALWAYS ON
  DEPENDS libgfx-native
)

set(GFXTOOL "${LUNA_INSTALL_NATIVE_DIR}/bin/gfxtool")

ExternalProject_Add(
  xxd-native
  PREFIX "${LUNA_BUILD_NATIVE_DIR}"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_PREFIX_PATH=${LUNA_INSTALL_NATIVE_DIR}
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  ${EXT_PROJ_TOOLCHAIN}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools/xxd
  BUILD_ALWAYS ON
)

set(XXD "${LUNA_INSTALL_NATIVE_DIR}/bin/xxd")
