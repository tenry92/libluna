#pragma once

#include <libluna/config.h>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#endif

#include <libluna/Canvas.hpp>
#include <libluna/ImmediateGui.hpp>

class Luna::ImmediateGuiImpl {
  public:
  void init(Canvas *canvas);
  void newFrame();
  void render(ImmediateGui *gui);
#ifdef LUNA_WINDOW_SDL2
  bool processSdlEvent(const SDL_Event *event);
#endif
  Canvas *mCanvas{nullptr};
#ifdef LUNA_IMGUI
  ImGuiContext *mImGuiContext{nullptr};
#endif
};
