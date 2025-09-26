# add_subdirectory(libs/assimp-5.4.2)

if(LUNA_USE_STD_THREAD)
  find_package(Threads REQUIRED)
  target_link_libraries(luna PUBLIC Threads::Threads)
endif()

if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Nintendo64" OR CMAKE_SYSTEM_NAME STREQUAL "NintendoDS"))
  find_package(glm REQUIRED)
  target_link_libraries(luna PUBLIC glm::glm)
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
  add_library(imgui_base OBJECT
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui.cpp
  )
  target_include_directories(imgui_base PRIVATE libs/imgui)
  target_sources(luna PRIVATE $<TARGET_OBJECTS:imgui_base>)
  target_include_directories(luna PRIVATE libs/imgui)

  if(LUNA_RENDERER_OPENGL)
    add_library(imgui_opengl OBJECT
      libs/imgui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui_opengl PRIVATE libs/imgui)
    target_sources(luna PRIVATE $<TARGET_OBJECTS:imgui_opengl>)
  endif()

  if(LUNA_WINDOW_SDL2)
    add_library(imgui_sdl2 OBJECT
      libs/imgui/backends/imgui_impl_sdl2.cpp
      libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
    )
    target_include_directories(imgui_sdl2 PRIVATE libs/imgui)
    target_link_libraries(imgui_sdl2 PRIVATE SDL2::SDL2)
    target_sources(luna PRIVATE $<TARGET_OBJECTS:imgui_sdl2>)
  endif()

  if(LUNA_WINDOW_GLFW)
    add_library(imgui_glfw OBJECT
      libs/imgui/backends/imgui_impl_glfw.cpp
    )
    target_include_directories(imgui_glfw PRIVATE libs/imgui)
    target_link_libraries(imgui_glfw PRIVATE glfw)
    target_sources(luna PRIVATE $<TARGET_OBJECTS:imgui_glfw>)
  endif()
endif()

# target_include_directories(luna PUBLIC
#   /usr/include/opus
# )

if(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  target_link_libraries(luna PRIVATE EGL glapi drm_nouveau)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  target_include_directories(luna PRIVATE ${LIBDRAGON_INCLUDE_DIR})
  # target_link_libraries(luna PRIVATE ${LIBDRAGON_LIB})
endif()

target_include_directories(luna PUBLIC
  /usr/local/include
)

if(CMAKE_SYSTEM_NAME IN_LIST DESKTOP)
  add_subdirectory(libs/glad-4.3)
  target_sources(luna PRIVATE
    $<TARGET_OBJECTS:glad>
  )
  target_include_directories(luna PRIVATE libs/glad-4.3/include)
endif()

set(FMT_OS OFF)

if(CMAKE_SYSTEM_NAME IN_LIST DESKTOP)
  find_package(fmt REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.0.0
  )
  set(FMT_INSTALL ON)
  FetchContent_MakeAvailable(fmt)
endif()

target_link_libraries(luna PUBLIC fmt::fmt-header-only)
