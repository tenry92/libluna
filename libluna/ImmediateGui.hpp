#pragma once

#include <libluna/config.h>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#endif

#include <libluna/ImmediateGui.hpp>

namespace Luna {
  class Canvas;

  class ImmediateGui {
    public:
    ImmediateGui();
    virtual ~ImmediateGui();

    protected:
    virtual void render() = 0;

    private:
    friend class Canvas;
    void init(Canvas* canvas);
    void newFrame();
    void render(ImmediateGui* gui);
#ifdef LUNA_WINDOW_SDL2
    static bool processSdlEvent(const SDL_Event* event);
#endif
    Canvas* mCanvas{nullptr};
#ifdef LUNA_IMGUI
    ImGuiContext* mImGuiContext{nullptr};
#endif
  };
} // namespace Luna
