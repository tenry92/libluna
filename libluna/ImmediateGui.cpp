#include <libluna/config.h>

#include <libluna/ImmediateGui.hpp>
#include <libluna/ImmediateGuiImpl.hpp>

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>

#ifdef LUNA_WINDOW_SDL2
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#endif
#endif // IMGUI

#include <libluna/CanvasImpl.hpp>

using namespace Luna;

void ImmediateGuiImpl::init(Canvas *canvas) { mCanvas = canvas; }

void ImmediateGuiImpl::newFrame() {
  // stub
}

void ImmediateGuiImpl::render(ImmediateGui *gui) { gui->render(); }

#ifdef LUNA_WINDOW_SDL2
bool ImmediateGuiImpl::processSdlEvent([[maybe_unused]] const SDL_Event *event
) {
#ifdef LUNA_IMGUI
  return ImGui_ImplSDL2_ProcessEvent(event);
#else
  return false;
#endif
}
#endif

ImmediateGui::ImmediateGui() : mImpl{std::make_unique<ImmediateGuiImpl>()} {}

ImmediateGui::~ImmediateGui() = default;

ImmediateGuiImpl *ImmediateGui::getImpl() const { return mImpl.get(); }
