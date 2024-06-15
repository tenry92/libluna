#pragma once

#include <libluna/Renderers/CommonRenderer.hpp>

#include <memory>

namespace Luna {
  class OpenglRenderer : public CommonRenderer {
    public:
    OpenglRenderer();
    ~OpenglRenderer() override;

    void initialize() override;
    void initializeImmediateGui() override;
    void quitImmediateGui() override;
    void close() override;
    void present() override;
    Internal::GraphicsMetrics getMetrics() override;

    void clearBackground(Color color) override;
    void createTexture(int id) override;
    void destroyTexture(int id) override;
    void loadTexture(int id, std::shared_ptr<Image> image, int frameIndex) override;
    void resizeTexture(int id, Vector2i size) override;
    void renderTexture(Canvas *canvas, RenderTextureInfo *info) override;

    void createMesh(int id) override;
    void destroyMesh(int id) override;
    void loadMesh(int id, std::shared_ptr<Mesh> mesh) override;

    void renderMesh(Canvas *canvas, RenderMeshInfo *info) override;

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
