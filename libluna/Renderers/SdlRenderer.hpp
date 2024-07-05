#pragma once

#include <libluna/Renderers/CommonRenderer.hpp>

#include <memory>

namespace Luna {
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
    void loadTexture(int id, ImagePtr image) override;
    void resizeTexture(int id, Vector2i size) override;
    void renderTexture(Canvas *canvas, RenderTextureInfo *info) override;

    void setTextureFilterEnabled(int id, bool enabled) override;
    void setRenderTargetTexture(int id) override;
    void unsetRenderTargetTexture() override;

    void setViewport(Vector2i offset, Vector2i size) override;

    void imguiNewFrame() override;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
