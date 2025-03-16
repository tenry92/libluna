#include <libluna/config.h>

#include <libluna/ImmediateGui.hpp>
#include <libluna/Logger.hpp>

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>

#ifdef LUNA_WINDOW_SDL2
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#endif
#endif // IMGUI

using namespace Luna;

void ImmediateGui::init(Canvas* canvas) { mCanvas = canvas; }

void ImmediateGui::newFrame() {
  // stub
}

void ImmediateGui::render(ImmediateGui* gui) { gui->render(); }

#ifdef LUNA_WINDOW_SDL2
bool ImmediateGui::processSdlEvent([[maybe_unused]] const SDL_Event* event) {
#ifdef LUNA_IMGUI
  ImGui_ImplSDL2_ProcessEvent(event);
  return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
#else
  return false;
#endif
}
#endif

ImmediateGui::ImmediateGui() = default;

ImmediateGui::~ImmediateGui() = default;
