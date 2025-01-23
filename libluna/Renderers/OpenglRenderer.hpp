#pragma once

#include <libluna/config.h>

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#endif

#include <libluna/GL/MeshBuffer.hpp>
#include <libluna/GL/Shader.hpp>
#include <libluna/GL/ShaderLib.hpp>
#include <libluna/GL/SpriteBuffer.hpp>
#include <libluna/GL/Uniform.hpp>
#include <libluna/GL/common.hpp>

#include <libluna/Renderers/CommonRenderer.hpp>

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

    void clearBackground(ColorRgb color) override;
    void createTexture(int id) override;
    void destroyTexture(int id) override;
    void loadTexture(int id, Image* image) override;
    void resizeTexture(int id, Vector2i size) override;
    void renderTexture(Canvas* canvas, RenderTextureInfo* info) override;

    void createShape(int id) override;
    void destroyShape(int id) override;
    void loadShape(int id, Shape* shape) override;
    void renderShape(Canvas* canvas, RenderShapeInfo* info) override;

    void createMesh(int id) override;
    void destroyMesh(int id) override;
    void loadMesh(int id, std::shared_ptr<Mesh> mesh) override;

    void renderMesh(Canvas* canvas, RenderMeshInfo* info) override;

    void setTextureFilterEnabled(int id, bool enabled) override;
    void setRenderTargetTexture(int id) override;
    void unsetRenderTargetTexture() override;

    void setViewport(Vector2i offset, Vector2i size) override;

    void imguiNewFrame() override;

    private:
#ifdef LUNA_IMGUI
    ImGuiContext* mImGuiContext{nullptr};
#endif

    GL::Shader mSpriteShader;
    GL::Shader mPrimitiveShader;
    GL::Shader mModelShader;
    // std::unique_ptr<TextureCacheType> mTextureCache;
    std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

    struct {
      GL::Uniform screenSize;
      GL::Uniform spritePos;
      GL::Uniform spriteTexture;
      GL::Uniform primitiveColor;
      GL::Uniform uPrimitivePos;
      GL::Uniform transformModel;
      GL::Uniform transformView;
      GL::Uniform transformProjection;
      GL::Uniform materialDiffuseMap;
      GL::Uniform materialNormalMap;
      GL::Uniform ambientLightColor;
      GL::Uniform ambientLightIntensity;
      GL::Uniform pointLightsEnabled;
      GL::Uniform pointLightsColor;
      GL::Uniform pointLightsPosition;
      GL::Uniform viewPos;
    } mUniforms;

    std::map<int, GLuint> mTextureIdMapping;
    std::map<int, Luna::Shape*> mShapeIdMapping;
    std::map<int, GLuint> mFramebuffers;
    std::map<int, std::shared_ptr<GL::MeshBuffer>> mMeshMapping;
    bool mUsingFramebuffer{false};
  };
} // namespace Luna
