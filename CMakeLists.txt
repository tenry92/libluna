cmake_minimum_required(VERSION 3.12)

project(libluna
  VERSION 0.1.0.0
  DESCRIPTION "Luna Framework"
  LANGUAGES C CXX
)

include(ExternalProject)

# set(CMAKE_MODULE_PATH "/opt/cmake/module")
# set(CMAKE_INSTALL_PREFIX "${CMAKE_PREFIX_PATH}")

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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs> # for imgui
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files like config.h

  $<INSTALL_INTERFACE:include>
)

if(CMAKE_SYSTEM_NAME STREQUAL "NintendoDS")
  add_compile_definitions(NDS __NDS__)
endif()

################################################################################
# SOURCES

set(LUNA_SOURCES
  libluna/AbstractRenderer.cpp
  libluna/Application.cpp
  libluna/Audio/AudioManager.cpp
  libluna/Audio/AudioNode.cpp
  libluna/Audio/DelayNode.cpp
  libluna/Audio/GainNode.cpp
  libluna/Audio/OscillatorNode.cpp
  libluna/ButtonEvent.cpp
  libluna/Camera2d.cpp
  libluna/Camera3d.cpp
  libluna/Canvas.cpp
  libluna/Console.cpp
  libluna/Drawable2d.cpp
  libluna/Filesystem/FileReader.cpp
  libluna/Filesystem/Path.cpp
  libluna/Font.cpp
  libluna/Texture.cpp
  libluna/ImmediateGui.cpp
  libluna/Input/GenericGamepadDevice.cpp
  libluna/Input/KeyboardDevice.cpp
  libluna/Input/N3dsGamepadDevice.cpp
  libluna/Input/N64GamepadDevice.cpp
  libluna/Input/NdsGamepadDevice.cpp
  libluna/Input/Ps5GamepadDevice.cpp
  libluna/Input/SwitchProGamepadDevice.cpp
  libluna/Input/Xbox360GamepadDevice.cpp
  libluna/Input/XboxOneGamepadDevice.cpp
  libluna/InputDevice.cpp
  libluna/InputManager.cpp
  libluna/IntervalManager.cpp
  libluna/Logger.cpp
  libluna/Material.cpp
  libluna/Matrix.cpp
  libluna/MemoryReader.cpp
  libluna/Mesh.cpp
  libluna/MeshBuilder.cpp
  libluna/Model.cpp
  libluna/PathManager.cpp
  libluna/Performance/Ticker.cpp
  libluna/Performance/Timer.cpp
  libluna/Platform.cpp
  libluna/Primitive.cpp
  libluna/Rect.cpp
  libluna/Renderers/CommonRenderer.cpp
  libluna/ResourceReader.cpp
  libluna/Shape.cpp
  libluna/Sound.cpp
  libluna/Sprite.cpp
  libluna/Stage.cpp
  libluna/String.cpp
  libluna/System.cpp
  libluna/Text.cpp
  libluna/Tilemap.cpp
  libluna/Tileset.cpp
  libluna/Vector.cpp
)

set(LUNA_PUBLIC_HEADERS
  libluna/AbstractRenderer.hpp
  libluna/Application.hpp
  libluna/Audio/AudioManager.hpp
  libluna/Audio/AudioNode.hpp
  libluna/Audio/DelayNode.hpp
  libluna/Audio/GainNode.hpp
  libluna/Audio/OscillatorNode.hpp
  libluna/ButtonEvent.hpp
  libluna/Camera2d.hpp
  libluna/Camera3d.hpp
  libluna/Canvas.hpp
  libluna/Color.hpp
  libluna/Console.hpp
  libluna/Drawable2d.hpp
  libluna/Filesystem/FileReader.hpp
  libluna/Filesystem/Path.hpp
  libluna/Font.hpp
  libluna/ImmediateGui.hpp
  libluna/Input/GenericGamepadDevice.hpp
  libluna/Input/KeyboardDevice.hpp
  libluna/Input/N3dsGamepadDevice.hpp
  libluna/Input/N64GamepadDevice.hpp
  libluna/Input/NdsGamepadDevice.hpp
  libluna/Input/Ps5GamepadDevice.hpp
  libluna/Input/SwitchProGamepadDevice.hpp
  libluna/Input/Xbox360GamepadDevice.hpp
  libluna/Input/XboxOneGamepadDevice.hpp
  libluna/InputDevice.hpp
  libluna/InputManager.hpp
  libluna/InputStream.hpp
  libluna/Internal/AudioMetrics.hpp
  libluna/Internal/DebugGui.hpp
  libluna/Internal/DebugMetrics.hpp
  libluna/Internal/GraphicsMetrics.hpp
  libluna/Internal/Keyboard.hpp
  libluna/IntervalManager.hpp
  libluna/Logger.hpp
  libluna/Material.hpp
  libluna/Matrix.hpp
  libluna/MemoryReader.hpp
  libluna/Mesh.hpp
  libluna/MeshBuilder.hpp
  libluna/Model.hpp
  libluna/Palette.hpp
  libluna/PathManager.hpp
  libluna/Performance/Ticker.hpp
  libluna/Performance/Timer.hpp
  libluna/Platform.hpp
  libluna/Pool.hpp
  libluna/Primitive.hpp
  libluna/Rect.hpp
  libluna/ResourceReader.hpp
  libluna/Shape.hpp
  libluna/Sound.hpp
  libluna/Sprite.hpp
  libluna/Stage.hpp
  libluna/String.hpp
  libluna/System.hpp
  libluna/Text.hpp
  libluna/Texture.hpp
  libluna/Tilemap.hpp
  libluna/Tileset.hpp
  libluna/utf8.h
  libluna/Vector.hpp
)

if(LUNA_IMGUI)
  list(APPEND LUNA_SOURCES libluna/Internal/DebugGui.cpp)
endif()

if(LUNA_RENDERER_SDL2)
  list(APPEND LUNA_SOURCES libluna/Renderers/SdlRenderer.cpp)
endif()

if(LUNA_RENDERER_OPENGL)
  list(APPEND LUNA_SOURCES libluna/Renderers/OpenglRenderer.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NintendoDS")
  list(APPEND LUNA_SOURCES libluna/Renderers/NdsRenderer.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  list(APPEND LUNA_SOURCES libluna/Renderers/N64Renderer.cpp)
endif()

target_sources(luna PRIVATE ${LUNA_SOURCES})

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
  primitive_vert.glsl
  primitive_frag.glsl
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
  EXPORT lunaTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include/libluna
)

foreach(header ${LUNA_PUBLIC_HEADERS})
  get_filename_component(header_dir ${header} DIRECTORY)
  install(FILES ${header} DESTINATION "include/${header_dir}")
endforeach()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libluna/config.h DESTINATION "include/libluna")
install(FILES libs/imgui/imconfig.h libs/imgui/imgui.h DESTINATION "include/libluna/imgui")

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/lunaConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/lunaConfig.cmake
  INSTALL_DESTINATION lib/cmake/luna
)

install(EXPORT lunaTargets
  FILE LunaTargets.cmake
  NAMESPACE luna::
  DESTINATION lib/cmake/luna
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lunaConfig.cmake
  DESTINATION lib/cmake/luna
)

# configure_file(luna.pc.in luna.pc @ONLY)

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/luna.pc
#   DESTINATION lib/pkgconfig
# )

################################################################################
# UNIT TESTS

if(LUNA_BUILD_TESTS)
  include(cmake/unit-tests.cmake)
endif()

################################################################################
# EXAMPLES

if(LUNA_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
