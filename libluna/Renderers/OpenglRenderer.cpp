#include <libluna/config.h>

#ifdef LUNA_RENDERER_OPENGL
#include <libluna/Renderers/OpenglRenderer.hpp>

#include <list>
#include <map>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_WINDOW_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_WINDOW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifdef LUNA_IMGUI
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#ifdef LUNA_WINDOW_SDL2
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#endif

#ifdef LUNA_WINDOW_GLFW
#include <imgui/backends/imgui_impl_glfw.h>
#endif
#endif // IMGUI

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/Logger.hpp>
#include <libluna/MemoryReader.hpp>

#include <libluna/CanvasImpl.hpp>

#include <libluna/GL/common.hpp>
#include <libluna/GL/MeshBuffer.hpp>
#include <libluna/GL/Shader.hpp>
#include <libluna/GL/ShaderLib.hpp>
#include <libluna/GL/SpriteBuffer.hpp>
#include <libluna/GL/Uniform.hpp>

#include <libluna/GL/shaders/3d_frag.glsl.h>
#include <libluna/GL/shaders/3d_vert.glsl.h>
#include <libluna/GL/shaders/common3d.glsl.h>
#include <libluna/GL/shaders/sprite_frag.glsl.h>
#include <libluna/GL/shaders/sprite_vert.glsl.h>

using namespace Luna;

class OpenglRenderer::impl {
  public:
#ifdef LUNA_IMGUI
  ImGuiContext *mImGuiContext{nullptr};
#endif

  GL::Shader mSpriteShader;
  GL::Shader mModelShader;
  // std::unique_ptr<TextureCacheType> mTextureCache;
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

  struct {
    GL::Uniform screenSize;
    GL::Uniform spritePos;
    GL::Uniform spriteTexture;
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
  std::map<int, GLuint> mFramebuffers;
  std::map<int, std::shared_ptr<GL::MeshBuffer>> mMeshMapping;
  bool mUsingFramebuffer{false};
};

OpenglRenderer::OpenglRenderer() : mImpl{std::make_unique<impl>()} {
  mImpl->mMetrics = std::make_shared<Internal::GraphicsMetrics>();
}

OpenglRenderer::~OpenglRenderer() = default;

void OpenglRenderer::initialize() {
  if (!gladLoadGL()) {
    logError("gladLoadGL() failed");
  }

  glGetIntegerv(
      GL_MAX_RECTANGLE_TEXTURE_SIZE, &mImpl->mMetrics->maxTextureSize
  );
  glGetIntegerv(GL_MAJOR_VERSION, &mImpl->mMetrics->glMajor);
  glGetIntegerv(GL_MINOR_VERSION, &mImpl->mMetrics->glMinor);
  mImpl->mMetrics->vendor =
      reinterpret_cast<const char *>(glGetString(GL_VENDOR));
  mImpl->mMetrics->shadingLangVersion =
      reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

  GL::ShaderLib shaderLib;
  shaderLib.registerShader(
      "sprite_vert.glsl",
      std::make_unique<MemoryReader>(sprite_vert_glsl, sprite_vert_glsl_len)
  );
  shaderLib.registerShader(
      "sprite_frag.glsl",
      std::make_unique<MemoryReader>(sprite_frag_glsl, sprite_frag_glsl_len)
  );
  shaderLib.registerShader(
      "3d_vert.glsl",
      std::make_unique<MemoryReader>(__3d_vert_glsl, __3d_vert_glsl_len)
  );
  shaderLib.registerShader(
      "3d_frag.glsl",
      std::make_unique<MemoryReader>(__3d_frag_glsl, __3d_frag_glsl_len)
  );
  shaderLib.registerShader(
      "common3d.glsl",
      std::make_unique<MemoryReader>(common3d_glsl, common3d_glsl_len)
  );

  mImpl->mSpriteShader =
      shaderLib.compileShader("sprite_vert.glsl", "sprite_frag.glsl");
  mImpl->mModelShader = shaderLib.compileShader("3d_vert.glsl", "3d_frag.glsl");
}

void OpenglRenderer::initializeImmediateGui() {
#ifdef LUNA_IMGUI
  IMGUI_CHECKVERSION();
  mImpl->mImGuiContext = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

#ifdef LUNA_USE_SDL
  ImGui_ImplSDL2_InitForOpenGL(
      getCanvas()->getImpl()->sdl.window, getCanvas()->getImpl()->sdl.glContext
  );
#endif

  ImGui_ImplOpenGL3_Init("#version 430 core");

  // todo(?): load fonts
#endif
}

void OpenglRenderer::quitImmediateGui() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplOpenGL3_Shutdown();
#ifdef LUNA_USE_SDL
    ImGui_ImplSDL2_Shutdown();
#endif
    ImGui::DestroyContext(mImpl->mImGuiContext);
    mImpl->mImGuiContext = nullptr;
  }
#endif
}

void OpenglRenderer::close() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplOpenGL3_Shutdown();

#ifdef LUNA_USE_SDL
    ImGui_ImplSDL2_Shutdown();
#endif
    ImGui::DestroyContext();
    mImpl->mImGuiContext = nullptr;
  }
#endif
}

void OpenglRenderer::present() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
#endif

#ifdef LUNA_WINDOW_SDL2
  SDL_GL_SwapWindow(getCanvas()->getImpl()->sdl.window);
#endif

#ifdef LUNA_WINDOW_GLFW
  glfwSwapBuffers(getCanvas()->getImpl()->glfw.window);
#endif

#ifdef LUNA_WINDOW_EGL
  eglSwapBuffers(getCanvas()->getImpl()->egl.display, getCanvas()->getImpl()->egl.surface);
#endif
}

Internal::GraphicsMetrics OpenglRenderer::getMetrics() {
  return *mImpl->mMetrics;
}


void OpenglRenderer::clearBackground(ColorRgb color) {
  CHECK_GL(glClearColor(color.red, color.green, color.blue, color.alpha));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenglRenderer::createTexture(int id) {
  GLuint texture;
  CHECK_GL(glGenTextures(1, &texture));
  mImpl->mTextureIdMapping.emplace(id, texture);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void OpenglRenderer::destroyTexture(int id) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);
  mImpl->mTextureIdMapping.erase(id);
  CHECK_GL(glDeleteTextures(1, &texture));
}

void OpenglRenderer::loadTexture(int id, ImagePtr image) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);

  GLenum inputFormat = GL_RGBA;
  GLenum inputType = GL_UNSIGNED_BYTE;

  switch (image->getBitsPerPixel()) {
    case 16:
      inputFormat = GL_RGBA;
      inputType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
      break;
    case 24:
      inputFormat = GL_RGB;
      break;
    case 32:
      inputFormat = GL_RGBA;
      break;
  }

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glTexImage2D(
      GL_TEXTURE_2D, 0,                              /* mipmap level */
      GL_RGBA,                                       /* internal format */
      image->getSize().x(), image->getSize().y(), 0, /* format (legacy) */
      inputFormat,                                   /* input format */
      inputType, image->getData()
  ));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenglRenderer::resizeTexture(int id, Vector2i size) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
    nullptr
  ));
}

void OpenglRenderer::renderTexture([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info) {
  mImpl->mSpriteShader.use();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  [[maybe_unused]] auto screenSize = getCurrentRenderSize();
  mImpl->mUniforms.screenSize = mImpl->mSpriteShader.getUniform("uScreenSize");
  mImpl->mUniforms.screenSize =
      Vector2f(static_cast<float>(screenSize.x()), static_cast<float>(screenSize.y()));

  GLuint texture = mImpl->mTextureIdMapping.at(info->textureId);

  GL::SpriteBuffer spriteBuffer(info->size, !mImpl->mUsingFramebuffer);

  spriteBuffer.bind();

  mImpl->mUniforms.spriteTexture = mImpl->mSpriteShader.getUniform("uSpriteTexture");
  mImpl->mUniforms.spriteTexture = 0;
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));

  mImpl->mUniforms.spritePos = mImpl->mSpriteShader.getUniform("uSpritePos");
  mImpl->mUniforms.spritePos = info->position;

  spriteBuffer.draw();
}

void OpenglRenderer::createMesh([[maybe_unused]] int id) {
  // stub
}

void OpenglRenderer::destroyMesh([[maybe_unused]] int id) {
  mImpl->mMeshMapping.erase(id);
}

void OpenglRenderer::loadMesh([[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh) {
  auto meshBuffer = std::make_shared<GL::MeshBuffer>(mesh);

  mImpl->mMeshMapping.emplace(id, meshBuffer);
}

void OpenglRenderer::renderMesh([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info) {
  mImpl->mModelShader.use();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  auto ambientLight = canvas->getStage()->getAmbientLight();
  mImpl->mUniforms.ambientLightColor =
      mImpl->mModelShader.getUniform("uAmbientLight.color");
  mImpl->mUniforms.ambientLightIntensity =
      mImpl->mModelShader.getUniform("uAmbientLight.intensity");
  mImpl->mUniforms.ambientLightColor = ambientLight.color;
  mImpl->mUniforms.ambientLightIntensity = ambientLight.intensity;

  mImpl->mUniforms.pointLightsColor =
      mImpl->mModelShader.getUniform("uPointLights[{}].color", 1);
  mImpl->mUniforms.pointLightsPosition =
      mImpl->mModelShader.getUniform("uPointLights[{}].position", 1);

  for (auto &&pointLight : canvas->getStage()->getPointLights()) {
    mImpl->mUniforms.pointLightsColor[0] = pointLight->color;
    mImpl->mUniforms.pointLightsPosition[0] = pointLight->position;
  }

  auto mesh = mImpl->mMeshMapping.at(info->meshId);
  mesh->bind();

  GLuint diffuse = mImpl->mTextureIdMapping.at(info->diffuseTextureId);

  mImpl->mUniforms.materialDiffuseMap =
      mImpl->mModelShader.getUniform("uMaterial.diffuseMap");
  mImpl->mUniforms.materialDiffuseMap = 0;
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, diffuse));

  if (info->normalTextureId) {
    GLuint normal = mImpl->mTextureIdMapping.at(info->normalTextureId);
    mImpl->mUniforms.materialNormalMap =
        mImpl->mModelShader.getUniform("uMaterial.normalMap");
    mImpl->mUniforms.materialNormalMap = 1;

    glActiveTexture(GL_TEXTURE1);
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, normal));
    glActiveTexture(GL_TEXTURE0);
  }

  mImpl->mUniforms.transformModel = mImpl->mModelShader.getUniform("uTransform.model");
  mImpl->mUniforms.transformModel = info->transform;

  auto camera = canvas->getCamera3d();

  mImpl->mUniforms.transformView = mImpl->mModelShader.getUniform("uTransform.view");
  mImpl->mUniforms.transformView = camera.getViewMatrix();

  mImpl->mUniforms.transformProjection =
      mImpl->mModelShader.getUniform("uTransform.projection");
  mImpl->mUniforms.transformProjection = camera.getProjectionMatrix(static_cast<float>(getCurrentRenderSize().x()) / static_cast<float>(getCurrentRenderSize().y()));

  mesh->draw();
}

void OpenglRenderer::setTextureFilterEnabled([[maybe_unused]] int id, [[maybe_unused]] bool enabled) {

}

void OpenglRenderer::setRenderTargetTexture(int id) {
  GLuint framebuffer;

  if (mImpl->mFramebuffers.count(id) == 0) {
    CHECK_GL(glGenFramebuffers(1, &framebuffer));
    mImpl->mFramebuffers.emplace(id, framebuffer);
  } else {
    framebuffer = mImpl->mFramebuffers.at(id);
  }

  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

  GLuint texture = mImpl->mTextureIdMapping.at(id);
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, texture, 0
  ));

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    logWarn("framebuffer is not complete");
  }

  mImpl->mUsingFramebuffer = true;
}

void OpenglRenderer::unsetRenderTargetTexture() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  mImpl->mUsingFramebuffer = false;
}

void OpenglRenderer::setViewport(Vector2i offset, Vector2i size) {
  CHECK_GL(glViewport(offset.x(), offset.y(), size.x(), size.y()));
}


void OpenglRenderer::imguiNewFrame() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplOpenGL3_NewFrame();
#ifdef LUNA_WINDOW_SDL2
    ImGui_ImplSDL2_NewFrame();
#endif
    ImGui::NewFrame();
  }
#endif
}

#endif
