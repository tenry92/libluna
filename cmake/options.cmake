option(LUNA_BUILD_TESTS "Build tests" ON)
option(LUNA_BUILD_EXAMPLES "Build examples" ON)

set(LUNA_WINDOW ${LUNA_DEFAULT_WINDOW} CACHE STRING "Choose one of: sdl2, glfw, egl, none")
set_property(CACHE LUNA_WINDOW PROPERTY STRINGS "sdl2;glfw;egl;none")
if(LUNA_WINDOW STREQUAL "sdl2")
  set(LUNA_WINDOW_SDL2 ON)
endif()
if(LUNA_WINDOW STREQUAL "glfw")
  set(LUNA_WINDOW_GLFW ON)
endif()
if(LUNA_WINDOW STREQUAL "egl")
  set(LUNA_WINDOW_EGL ON)
endif()

option(LUNA_RENDERER_SDL2 "Enable SDL2 renderer" ${LUNA_SUPPORTS_SDL2})
option(LUNA_RENDERER_OPENGL "Enable OpenGL renderer" ${LUNA_SUPPORTS_OPENGL})
option(LUNA_RENDERER_N64_GL "Enable N64 OpenGL 1.1 renderer" ${NINTENDO_64})

option(LUNA_AUDIO_SDL2 "Enable audio via SDL2" ${LUNA_SUPPORTS_SDL2})

option(LUNA_IMGUI "Enable ImGui" ${SUPPORTS_IMGUI})
option(LUNA_STD_THREAD "Enable threading using std::thread" ${SUPPORTS_STD_THREAD})

configure_file(libluna/config.h.in libluna/config.h)
