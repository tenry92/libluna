cmake_minimum_required(VERSION 3.12)

project(libluna
  VERSION 0.1.0.0
  DESCRIPTION "Luna Game Engine"
  LANGUAGES C CXX
)

include(ExternalProject)

set(CMAKE_MODULE_PATH "/opt/cmake/module")
set(CMAKE_INSTALL_PREFIX "${CMAKE_PREFIX_PATH}")

################################################################################
# VARIABLES

set(LUNA_BUILD_NATIVE_DIR "${CMAKE_CURRENT_BINARY_DIR}/native")
set(LUNA_INSTALL_NATIVE_DIR "${CMAKE_CURRENT_BINARY_DIR}/native")

################################################################################
# METADATA

include(cmake/metadata.cmake)

################################################################################
# DETECT TARGET FEATURE SUPPORT

include(cmake/feature-support.cmake)

################################################################################
# OPTIONS

include(cmake/options.cmake)

################################################################################
# TOOLS

include(cmake/tools.cmake)

################################################################################
# TARGET CONFIGURATION

add_library(luna STATIC)
add_dependencies(luna shaders)
target_compile_features(luna PUBLIC cxx_std_17)
target_include_directories(luna PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/libs # for imgui
  ${CMAKE_CURRENT_BINARY_DIR} # for generated files like config.h
)

################################################################################
# SOURCES

# public modules (their .hpp files will be installed)
set(PUBLIC_MODULES
  AbstractRenderer
  Application
  Audio/AudioManager
  Audio/AudioNode
  Audio/DelayNode
  Audio/GainNode
  Audio/OscillatorNode
  ButtonEvent
  Camera2d
  Camera3d
  Canvas
  Console
  Filesystem/FileReader
  Filesystem/Path
  Font
  Image
  ImmediateGui
  InputManager
  IntervalManager
  Logger
  Material
  Matrix
  MemoryReader
  Mesh
  MeshBuilder
  Model
  PathManager
  Performance/Ticker
  Performance/Timer
  Platform
  ResourceReader
  Sprite
  Stage
  String
  System
  Text
  Util/Zlib
  Vector
)

set(PRIVATE_MODULES
  Renderers/CommonRenderer
)

# private modules (their .hpp files will NOT be installed)
if(LUNA_IMGUI)
  list(APPEND PRIVATE_MODULES
    Internal/DebugGui
  )
endif()

if(LUNA_RENDERER_SDL2)
  list(APPEND PRIVATE_MODULES Renderers/SdlRenderer)
endif()

if(LUNA_RENDERER_OPENGL)
  list(APPEND PRIVATE_MODULES Renderers/OpenglRenderer)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  list(APPEND PRIVATE_MODULES Renderers/N64Renderer)
endif()

# X => libluna/X.cpp
set(SOURCE_FILES ${PUBLIC_MODULES} ${PRIVATE_MODULES})
list(TRANSFORM SOURCE_FILES PREPEND "libluna/")
list(TRANSFORM SOURCE_FILES APPEND ".cpp")

# X => libluna/X.hpp
set(PUBLIC_HEADERS ${PUBLIC_MODULES})
list(TRANSFORM PUBLIC_HEADERS PREPEND "libluna/")
list(TRANSFORM PUBLIC_HEADERS APPEND ".hpp")

target_sources(luna PRIVATE
  ${SOURCE_FILES}
)

list(APPEND PUBLIC_HEADERS
  libluna/Color.hpp
  libluna/InputStream.hpp
  libluna/Palette.hpp
  libluna/Resource.hpp
  libs/imgui/imgui.h
  libs/imgui/imconfig.h
)

# enable warnings and debug
if(MSVC)
  target_compile_options(luna PRIVATE /W4 /WX)

  set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /DNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /DNDEBUG")
else()
  target_compile_options(luna PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Werror -Wnull-dereference -Wconversion)

  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
endif()

################################################################################
# DEPENDENCIES

include(cmake/dependencies.cmake)

################################################################################
# GLSL SHADERS

set(SHADER_SOURCE_FILES
  sprite_vert.glsl
  sprite_frag.glsl
  3d_vert.glsl
  3d_frag.glsl
  common3d.glsl
)

foreach(SHADER_SOURCE_FILE ${SHADER_SOURCE_FILES})
  get_filename_component(SHADER_NAME ${SHADER_SOURCE_FILE} NAME)
  set(SHADER_HEADER_FILE "${PROJECT_BINARY_DIR}/libluna/GL/shaders/${SHADER_SOURCE_FILE}.h")
  add_custom_command(
    OUTPUT ${SHADER_HEADER_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/libluna/GL/shaders
    COMMAND ${XXD} -n ${SHADER_SOURCE_FILE} -i ${PROJECT_SOURCE_DIR}/libluna/GL/shaders/${SHADER_SOURCE_FILE} ${SHADER_HEADER_FILE}
    DEPENDS xxd-native ${PROJECT_SOURCE_DIR}/libluna/GL/shaders/${SHADER_SOURCE_FILE}
    COMMENT "Converting ${SHADER_SOURCE_FILE} to C header"
  )
  list(APPEND SHADER_HEADER_FILES "${SHADER_HEADER_FILE}")
endforeach()

add_custom_target(shaders ALL
  DEPENDS ${SHADER_HEADER_FILES}
)

add_dependencies(shaders xxd-native)

################################################################################
# INSTALL

install(TARGETS luna
  ARCHIVE
)

install(FILES ${PUBLIC_HEADERS} DESTINATION include/libluna)

################################################################################
# UNIT TESTS

include(cmake/unit-tests.cmake)

################################################################################
# EXAMPLES

add_subdirectory(examples)
