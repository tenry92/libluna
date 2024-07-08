if(LUNA_USE_STD_THREAD)
  find_package(Threads REQUIRED)
  target_link_libraries(luna PUBLIC Threads::Threads)
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  find_package(ZLIB REQUIRED)
  target_link_libraries(luna PUBLIC ZLIB::ZLIB)

  find_path(OPUSFILE_INCLUDE_DIR NAMES opusfile.h REQUIRED PATH_SUFFIXES opus)
  find_library(OGG_LIB NAMES ogg)
  find_library(OPUS_LIB NAMES opus)
  find_library(OPUSFILE_LIB NAMES opusfile)
  target_include_directories(luna PRIVATE ${OPUSFILE_INCLUDE_DIR})
  target_link_libraries(luna PRIVATE ${OPUSFILE_LIB} ${OPUS_LIB} ${OGG_LIB})

  find_path(UTF8_INCLUDE_DIR NAMES utf8.h REQUIRED HINTS ${PROJECT_SOURCE_DIR}/libs PATH_SUFFIXES utf8)
  find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp REQUIRED PATH_SUFFIXES utf8)
  target_include_directories(luna PRIVATE ${UTF8_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
endif()

if(LUNA_WINDOW_SDL2)
  find_package(SDL2 REQUIRED)
  target_link_libraries(luna PUBLIC SDL2::SDL2)
endif()

if(LUNA_WINDOW_GLFW)
  find_package(glfw3 REQUIRED)
  target_link_libraries(luna PUBLIC glfw)
endif()

if(LUNA_RENDERER_OPENGL)
  if(UNIX)
    target_link_libraries(luna PUBLIC dl)
  endif()
endif()

if(LUNA_IMGUI)
  add_library(imgui_base STATIC
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui.cpp
  )
  target_include_directories(imgui_base PUBLIC libs/imgui)
  target_link_libraries(luna PRIVATE imgui_base)

  if(LUNA_RENDERER_OPENGL)
    add_library(imgui_opengl STATIC
      libs/imgui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui_opengl PUBLIC libs/imgui)
    target_link_libraries(luna PRIVATE imgui_opengl)
  endif()

  if(LUNA_WINDOW_SDL2)
    add_library(imgui_sdl2 STATIC
      libs/imgui/backends/imgui_impl_sdl2.cpp
      libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
    )
    target_include_directories(imgui_sdl2 PUBLIC libs/imgui)
    target_link_libraries(imgui_sdl2 PUBLIC SDL2::SDL2)
    target_link_libraries(luna PRIVATE imgui_sdl2)
  endif()

  if(LUNA_WINDOW_GLFW)
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

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  target_link_libraries(luna PRIVATE glad)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  target_link_libraries(luna PRIVATE EGL glapi drm_nouveau)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  target_include_directories(luna PRIVATE ${LIBDRAGON_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libs/utf8)
  # target_link_libraries(luna PRIVATE ${LIBDRAGON_LIB})
endif()

target_include_directories(luna PUBLIC
  /usr/local/include
)

if(CMAKE_SYSTEM_NAME IN_LIST DESKTOP)
  add_subdirectory(libs/glad-4.3)
endif()

add_subdirectory(libs/libgfx)
