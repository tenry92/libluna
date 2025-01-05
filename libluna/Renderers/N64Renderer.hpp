#pragma once

#include <map>
#include <vector>

#include <GL/gl.h>
#include <GL/gl_integration.h>
#include <GL/glu.h>

#include <libluna/Math.hpp>
#include <libluna/Renderers/CommonRenderer.hpp>

namespace Luna {
  class N64Renderer : public CommonRenderer {
    public:
    struct ChunkedTexture {
      std::vector<GLuint> ids;
      Vector2i size;
    };

    N64Renderer();
    ~N64Renderer() override;

    void startRender() override;
    void endRender() override;

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
    std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

    std::map<int, ChunkedTexture> mTextureIdMapping;
    std::map<int, Luna::Shape *> mShapeIdMapping;
    std::map<int, GLuint> mMeshIdMapping;
  };
} // namespace Luna
