cmake_minimum_required(VERSION 3.12)

project(libluna
  VERSION 0.1.0.0
  DESCRIPTION "Luna Game Engine"
  LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH "/opt/cmake/module")
set(CMAKE_INSTALL_PREFIX "${CMAKE_PREFIX_PATH}")

################################################################################
# METADATA

set(APP_AUTHOR "Tenry")
set(APP_NAME "${PROJECT_NAME}")
set(APP_PRODUCT_NAME "Luna Game Engine")
set(APP_DESCRIPTION "Luna Game Engine")
set(APP_ORIGINAL_FILENAME "${APP_NAME}")

################################################################################
# DETECT TARGET FEATURE SUPPORT

if(UNIX OR WIN32)
  set(SUPPORTS_SDL ON)
  set(SUPPORTS_OPENGL ON)
else()
  set(SUPPORTS_SDL OFF)
  set(SUPPORTS_OPENGL OFF)
endif()

if(UNIX OR WIN32)
  set(SUPPORTS_STD_THREAD ON)
else()
  set(SUPPORTS_STD_THREAD OFF)
endif()

if(UNIX OR WIN32)
  set(SUPPORTS_IMGUI ON)
else()
  set(SUPPORTS_IMGUI OFF)
endif()

################################################################################
# OPTIONS

option(LUNA_USE_SDL "Use SDL2 for Window creation, input handling and audio output" ${SUPPORTS_SDL})
option(LUNA_USE_GLFW "Use GLFW for Window creation and input handling" OFF)
option(LUNA_USE_OPENGL "Use OpenGL" ${SUPPORTS_OPENGL})
option(LUNA_USE_EGL "Use EGL" OFF)
option(LUNA_USE_STD_THREAD "Use std::thread" ${SUPPORTS_STD_THREAD})
option(LUNA_USE_IMGUI "Use ImGui" ${SUPPORTS_IMGUI})

################################################################################
# CONFIGURE

configure_file(libluna/config.h.in libluna/config.h)

################################################################################
# TOOLS

add_subdirectory(tools/xxd)
set(XXD xxd)

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
  AudioManager
  AudioNode
  ButtonEvent
  Camera2d
  Camera3d
  Canvas
  Filesystem/FileReader
  Filesystem/Path
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
  Terminal
  Util/Png
  Util/Zlib
  Vector
)

set(PRIVATE_MODULES
  Renderers/CommonRenderer
)

# private modules (their .hpp files will NOT be installed)
if(LUNA_USE_IMGUI)
  list(APPEND PRIVATE_MODULES
    Internal/DebugGui
  )
endif()

if(LUNA_USE_SDL)
  list(APPEND PRIVATE_MODULES Renderers/SdlRenderer)
endif()

if(LUNA_USE_OPENGL)
  list(APPEND PRIVATE_MODULES Renderers/OpenglRenderer)
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
  libluna/ResourceRef.hpp
  libs/imgui/imgui.h
  libs/imgui/imconfig.h
)

# enable warnings
if(MSVC)
  target_compile_options(luna PRIVATE /W4 /WX)
else()
  target_compile_options(luna PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Werror -Wnull-dereference -Wconversion)
endif()

################################################################################
# DEPENDENCIES

if(LUNA_USE_STD_THREAD)
  find_package(Threads REQUIRED)
  target_link_libraries(luna PUBLIC Threads::Threads)
endif()

find_package(ZLIB REQUIRED)
target_link_libraries(luna PUBLIC ZLIB::ZLIB)

find_package(PNG REQUIRED)
target_link_libraries(luna PUBLIC PNG::PNG)

find_path(OPUSFILE_INCLUDE_DIR NAMES opusfile.h REQUIRED PATH_SUFFIXES opus)
find_library(OGG_LIB NAMES ogg)
find_library(OPUS_LIB NAMES opus)
find_library(OPUSFILE_LIB NAMES opusfile)
target_include_directories(luna PRIVATE ${OPUSFILE_INCLUDE_DIR})
target_link_libraries(luna PRIVATE ${OPUSFILE_LIB} ${OPUS_LIB} ${OGG_LIB})

find_path(UTF8_INCLUDE_DIR NAMES utf8.h REQUIRED HINTS ${PROJECT_SOURCE_DIR}/libs PATH_SUFFIXES utf8)
find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp REQUIRED PATH_SUFFIXES utf8)
target_include_directories(luna PRIVATE ${UTF8_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

if(LUNA_USE_SDL)
  find_package(SDL2 REQUIRED)
  target_link_libraries(luna PUBLIC SDL2::SDL2)
endif()

if(LUNA_USE_GLFW)
  find_package(glfw3 REQUIRED)
  target_link_libraries(luna PUBLIC glfw)
endif()

if(LUNA_USE_OPENGL)
  if(UNIX)
    target_link_libraries(luna PUBLIC dl)
  endif()
endif()

if(LUNA_USE_IMGUI)
  add_library(imgui_base STATIC
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui.cpp
  )
  target_include_directories(imgui_base PUBLIC libs/imgui)
  target_link_libraries(luna PRIVATE imgui_base)

  if(LUNA_USE_OPENGL)
    add_library(imgui_opengl STATIC
      libs/imgui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui_opengl PUBLIC libs/imgui)
    target_link_libraries(luna PRIVATE imgui_opengl)
  endif()

  if(LUNA_USE_SDL)
    add_library(imgui_sdl2 STATIC
      libs/imgui/backends/imgui_impl_sdl2.cpp
      libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
    )
    target_include_directories(imgui_sdl2 PUBLIC libs/imgui)
    target_link_libraries(imgui_sdl2 PUBLIC SDL2::SDL2)
    target_link_libraries(luna PRIVATE imgui_sdl2)
  endif()

  if(LUNA_USE_GLFW)
    add_library(imgui_glfw STATIC
      libs/imgui/backends/imgui_impl_glfw.cpp
    )
    target_include_directories(imgui_glfw PUBLIC libs/imgui)
    target_link_libraries(imgui_glfw PUBLIC glfw)
    target_link_libraries(luna PRIVATE imgui_glfw)
  endif()
endif()

target_include_directories(luna PUBLIC
  /usr/include/opus
)

target_link_libraries(luna PRIVATE glad)

target_include_directories(luna PUBLIC
  /usr/local/include
)

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
    DEPENDS ${XXD} ${PROJECT_SOURCE_DIR}/libluna/GL/shaders/${SHADER_SOURCE_FILE}
    COMMENT "Converting ${SHADER_SOURCE_FILE} to C header"
  )
  list(APPEND SHADER_HEADER_FILES "${SHADER_HEADER_FILE}")
endforeach()

add_custom_target(shaders ALL
  DEPENDS ${SHADER_HEADER_FILES}
)

################################################################################
# INSTALL

install(TARGETS luna
  ARCHIVE
)

install(FILES ${PUBLIC_HEADERS} DESTINATION include/libluna)

################################################################################
# UNIT TESTS

set(UNIT_TESTS
  Filesystem/FileReader
  Filesystem/Path
  Image
  InputManager
  Matrix
  ResourceReader
  ResourceRef
  String
  Util/Png
  Util/Zlib
  Vector
)

foreach(test_name ${UNIT_TESTS})
  string(REPLACE "/" "_" TEST_TARGET_NAME "${test_name}.test")
  add_executable(${TEST_TARGET_NAME} libluna/${test_name}.test.cpp)

  add_dependencies(${TEST_TARGET_NAME} luna)
  target_link_libraries(${TEST_TARGET_NAME} PRIVATE luna)
endforeach()

################################################################################
# FUNCTIONAL/INTEGRATION TESTS

add_subdirectory(tests)

################################################################################
# THIRD PARTY

add_subdirectory(libs/glad-4.3)
