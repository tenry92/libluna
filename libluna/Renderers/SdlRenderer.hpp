#pragma once

#include <libluna/config.h>

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#endif

#include <SDL2/SDL.h>

#include <libluna/Logger.hpp>
#include <libluna/Renderers/CommonRenderer.hpp>

namespace Luna {
  struct SdlDeleter {
    void operator()(SDL_Renderer *renderer) const {
      logVerbose("destroying SDL renderer");
      SDL_DestroyRenderer(renderer);
    }

    void operator()(SDL_Texture *texture) const {
      logDebug("destroying SDL texture");
      SDL_DestroyTexture(texture);
    }
  };

  class SdlRenderer : public CommonRenderer {
    public:
    SdlRenderer();
    ~SdlRenderer() override;

    void initialize() override;
    void initializeImmediateGui() override;
    void quitImmediateGui() override;
    void close() override;
    void present() override;
    Internal::GraphicsMetrics getMetrics() override;

    void clearBackground(ColorRgb color) override;
    void createTexture(int id) override;
    void destroyTexture(int id) override;
    void loadTexture(int id, Image *image) override;
    void resizeTexture(int id, Vector2i size) override;
    void renderTexture(Canvas *canvas, RenderTextureInfo *info) override;

    void createShape(int id) override;
    void destroyShape(int id) override;
    void loadShape(int id, Shape *shape) override;
    void renderShape(Canvas *canvas, RenderShapeInfo *info) override;

    void setTextureFilterEnabled(int id, bool enabled) override;
    void setRenderTargetTexture(int id) override;
    void unsetRenderTargetTexture() override;

    void setViewport(Vector2i offset, Vector2i size) override;

    void imguiNewFrame() override;

    private:
#ifdef LUNA_IMGUI
    ImGuiContext *mImGuiContext{nullptr};
#endif

    std::unique_ptr<SDL_Renderer, SdlDeleter> mRenderer;
    std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

    std::map<int, SDL_Texture *> mTextureIdMapping;
    std::map<int, Luna::Shape *> mShapeIdMapping;
  };
} // namespace Luna
