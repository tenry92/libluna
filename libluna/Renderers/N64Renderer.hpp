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
    bool sliceTexture(
      Texture* texture, std::vector<Texture>& slices, Vector2i& sliceCount
    ) override;
    void createFramebufferTexture(uint16_t id, Vector2i size) override;
    void resizeFramebufferTexture(uint16_t id, Vector2i size) override;
    void destroyFramebufferTexture(uint16_t id) override;
    void freeTexture(int slot) override;
    void uploadTexture(int slot, const Texture* texture) override;
    void renderTexture(Canvas* canvas, RenderTextureInfo* info) override;

    void createShape(int id) override;
    void destroyShape(int id) override;
    void loadShape(int id, Shape* shape) override;
    void renderShape(Canvas* canvas, RenderShapeInfo* info) override;

    void createMesh(int id) override;
    void destroyMesh(int id) override;
    void loadMesh(int id, std::shared_ptr<Mesh> mesh) override;

    void renderMesh(Canvas* canvas, RenderMeshInfo* info) override;

    void setTextureFilterEnabled(uint16_t id, bool enabled) override;
    void setRenderTargetTexture(uint16_t id) override;
    void unsetRenderTargetTexture() override;

    void setViewport(Vector2i offset, Vector2i size) override;

    void imguiNewFrame() override;

    private:
    std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

    std::map<uint16_t, GLuint> mTextureIdMapping;
    std::map<int, Luna::Shape*> mShapeIdMapping;
    std::map<int, GLuint> mMeshIdMapping;
    GLuint mSpriteDisplayList{0};
  };
} // namespace Luna
