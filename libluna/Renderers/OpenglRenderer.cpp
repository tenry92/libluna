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

#include <libluna/Canvas.hpp>

#include <libluna/GL/MeshBuffer.hpp>
#include <libluna/GL/Shader.hpp>
#include <libluna/GL/ShaderLib.hpp>
#include <libluna/GL/SpriteBuffer.hpp>
#include <libluna/GL/Uniform.hpp>
#include <libluna/GL/common.hpp>

#include <libluna/GL/shaders/3d_frag.glsl.h>
#include <libluna/GL/shaders/3d_vert.glsl.h>
#include <libluna/GL/shaders/common3d.glsl.h>
#include <libluna/GL/shaders/primitive_frag.glsl.h>
#include <libluna/GL/shaders/primitive_vert.glsl.h>
#include <libluna/GL/shaders/sprite_frag.glsl.h>
#include <libluna/GL/shaders/sprite_vert.glsl.h>

using namespace Luna;

OpenglRenderer::OpenglRenderer() {
  mMetrics = std::make_shared<Internal::GraphicsMetrics>();
}

OpenglRenderer::~OpenglRenderer() = default;

void OpenglRenderer::initialize() {
  if (!gladLoadGL()) {
    logError("gladLoadGL() failed");
  }

  glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &mMetrics->maxTextureSize);
  glGetIntegerv(GL_MAJOR_VERSION, &mMetrics->glMajor);
  glGetIntegerv(GL_MINOR_VERSION, &mMetrics->glMinor);
  mMetrics->vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
  mMetrics->shadingLangVersion =
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
    "primitive_vert.glsl",
    std::make_unique<MemoryReader>(primitive_vert_glsl, primitive_vert_glsl_len)
  );
  shaderLib.registerShader(
    "primitive_frag.glsl",
    std::make_unique<MemoryReader>(primitive_frag_glsl, primitive_frag_glsl_len)
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

  mSpriteShader =
    shaderLib.compileShader("sprite_vert.glsl", "sprite_frag.glsl");
  mPrimitiveShader =
    shaderLib.compileShader("primitive_vert.glsl", "primitive_frag.glsl");
  mModelShader = shaderLib.compileShader("3d_vert.glsl", "3d_frag.glsl");
}

void OpenglRenderer::initializeImmediateGui() {
#ifdef LUNA_IMGUI
  IMGUI_CHECKVERSION();
  mImGuiContext = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

#ifdef LUNA_WINDOW_SDL2
  ImGui_ImplSDL2_InitForOpenGL(
    getCanvas()->sdl.window, getCanvas()->sdl.glContext
  );
#endif

  ImGui_ImplOpenGL3_Init("#version 430 core");

  // todo(?): load fonts
#endif
}

void OpenglRenderer::quitImmediateGui() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplOpenGL3_Shutdown();
#ifdef LUNA_WINDOW_SDL2
    ImGui_ImplSDL2_Shutdown();
#endif
    ImGui::DestroyContext(mImGuiContext);
    mImGuiContext = nullptr;
  }
#endif
}

void OpenglRenderer::close() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplOpenGL3_Shutdown();

#ifdef LUNA_WINDOW_SDL2
    ImGui_ImplSDL2_Shutdown();
#endif
    ImGui::DestroyContext();
    mImGuiContext = nullptr;
  }
#endif
}

void OpenglRenderer::present() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
#endif

#ifdef LUNA_WINDOW_SDL2
  SDL_GL_SwapWindow(getCanvas()->sdl.window);
#endif

#ifdef LUNA_WINDOW_GLFW
  glfwSwapBuffers(getCanvas()->glfw.window);
#endif

#ifdef LUNA_WINDOW_EGL
  eglSwapBuffers(getCanvas()->egl.display, getCanvas()->egl.surface);
#endif
}

Internal::GraphicsMetrics OpenglRenderer::getMetrics() { return *mMetrics; }

void OpenglRenderer::clearBackground(ColorRgb color) {
  CHECK_GL(glClearColor(color.red, color.green, color.blue, color.alpha));
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenglRenderer::createTexture(int id) {
  GLuint texture;
  CHECK_GL(glGenTextures(1, &texture));
  mTextureIdMapping.emplace(id, texture);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void OpenglRenderer::destroyTexture(int id) {
  GLuint texture = mTextureIdMapping.at(id);
  mTextureIdMapping.erase(id);
  CHECK_GL(glDeleteTextures(1, &texture));
}

void OpenglRenderer::loadTexture(int id, Image *image) {
  GLuint texture = mTextureIdMapping.at(id);

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
    GL_TEXTURE_2D, 0,                         /* mipmap level */
    GL_RGBA,                                  /* internal format */
    image->getWidth(), image->getHeight(), 0, /* format (legacy) */
    inputFormat,                              /* input format */
    inputType, image->getData()
  ));
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    image->isInterpolated() ? GL_LINEAR : GL_NEAREST
  );
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    image->isInterpolated() ? GL_LINEAR : GL_NEAREST
  );
}

void OpenglRenderer::resizeTexture(int id, Vector2i size) {
  GLuint texture = mTextureIdMapping.at(id);
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, nullptr
  ));
}

void OpenglRenderer::renderTexture(
  [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info
) {
  mSpriteShader.use();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  auto screenSize = getCurrentRenderSize();
  mUniforms.screenSize = mSpriteShader.getUniform("uScreenSize");
  mUniforms.screenSize = Vector2f(
    static_cast<float>(screenSize.width), static_cast<float>(screenSize.height)
  );

  GLuint texture = mTextureIdMapping.at(info->textureId);

  Rectf crop = {0.f, 0.f, 1.f, 1.f};

  if (info->crop.area() > 0) {
    auto tetureSize = getTextureSize(info->textureId);
    crop.x =
      static_cast<float>(info->crop.x) / static_cast<float>(tetureSize.width);
    crop.y =
      static_cast<float>(info->crop.y) / static_cast<float>(tetureSize.height);
    crop.width = static_cast<float>(info->crop.width) /
                 static_cast<float>(tetureSize.width);
    crop.height = static_cast<float>(info->crop.height) /
                  static_cast<float>(tetureSize.height);
  }

  GL::SpriteBuffer spriteBuffer(crop, info->size, !mUsingFramebuffer);

  spriteBuffer.bind();

  mUniforms.spriteTexture = mSpriteShader.getUniform("uSpriteTexture");
  mUniforms.spriteTexture = 0;
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));

  mUniforms.spritePos = mSpriteShader.getUniform("uSpritePos");
  mUniforms.spritePos = info->position;

  spriteBuffer.draw();
}

void OpenglRenderer::createMesh([[maybe_unused]] int id) {
  // stub
}

void OpenglRenderer::destroyMesh([[maybe_unused]] int id) {
  mMeshMapping.erase(id);
}

void OpenglRenderer::loadMesh(
  [[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh
) {
  auto meshBuffer = std::make_shared<GL::MeshBuffer>(mesh);

  mMeshMapping.emplace(id, meshBuffer);
}

void OpenglRenderer::renderMesh(
  [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info
) {
  mModelShader.use();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  auto ambientLight = canvas->getStage()->getAmbientLight();
  mUniforms.ambientLightColor = mModelShader.getUniform("uAmbientLight.color");
  mUniforms.ambientLightIntensity =
    mModelShader.getUniform("uAmbientLight.intensity");
  mUniforms.ambientLightColor = ambientLight.color;
  mUniforms.ambientLightIntensity = ambientLight.intensity;

  mUniforms.pointLightsColor =
    mModelShader.getUniform("uPointLights[{}].color", 1);
  mUniforms.pointLightsPosition =
    mModelShader.getUniform("uPointLights[{}].position", 1);

  for (auto &&pointLight : canvas->getStage()->getPointLights()) {
    mUniforms.pointLightsColor[0] = pointLight->color;
    mUniforms.pointLightsPosition[0] = pointLight->position;
  }

  auto mesh = mMeshMapping.at(info->meshId);
  mesh->bind();

  GLuint diffuse = mTextureIdMapping.at(info->diffuseTextureId);

  mUniforms.materialDiffuseMap =
    mModelShader.getUniform("uMaterial.diffuseMap");
  mUniforms.materialDiffuseMap = 0;
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, diffuse));

  if (info->normalTextureId) {
    GLuint normal = mTextureIdMapping.at(info->normalTextureId);
    mUniforms.materialNormalMap =
      mModelShader.getUniform("uMaterial.normalMap");
    mUniforms.materialNormalMap = 1;

    glActiveTexture(GL_TEXTURE1);
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, normal));
    glActiveTexture(GL_TEXTURE0);
  }

  mUniforms.transformModel = mModelShader.getUniform("uTransform.model");
  mUniforms.transformModel = info->transform;

  auto camera = canvas->getCamera3d();

  mUniforms.transformView = mModelShader.getUniform("uTransform.view");
  mUniforms.transformView = camera.getViewMatrix();

  mUniforms.transformProjection =
    mModelShader.getUniform("uTransform.projection");
  mUniforms.transformProjection = camera.getProjectionMatrix(
    static_cast<float>(getCurrentRenderSize().width) /
    static_cast<float>(getCurrentRenderSize().height)
  );

  mesh->draw();
}

void OpenglRenderer::createShape([[maybe_unused]] int id) {}

void OpenglRenderer::destroyShape([[maybe_unused]] int id) {
  mShapeIdMapping.erase(id);
}

void OpenglRenderer::loadShape(
  [[maybe_unused]] int id, [[maybe_unused]] Shape *shape
) {
  mShapeIdMapping.emplace(id, shape);
}

void OpenglRenderer::renderShape(
  [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderShapeInfo *info
) {
  auto shape = mShapeIdMapping.at(info->shapeId);

  mPrimitiveShader.use();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  auto screenSize = getCurrentRenderSize();
  mUniforms.screenSize = mPrimitiveShader.getUniform("uScreenSize");
  mUniforms.screenSize = Vector2f(
    static_cast<float>(screenSize.width), static_cast<float>(screenSize.height)
  );

  unsigned int vertexAttribConf;
  unsigned int pointBuffer;
  CHECK_GL(glGenVertexArrays(1, &vertexAttribConf));
  CHECK_GL(glGenBuffers(1, &pointBuffer));

  CHECK_GL(glBindVertexArray(vertexAttribConf));
  CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, pointBuffer));

  CHECK_GL(glVertexAttribPointer(
    0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0
  ));
  CHECK_GL(glEnableVertexAttribArray(0));

  std::vector<float> vertices;
  vertices.reserve(shape->getVertices().size() * 2);

  for (auto &&vertex : shape->getVertices()) {
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
  }

  CHECK_GL(glBufferData(
    GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
    GL_STATIC_DRAW
  ));

  mUniforms.primitiveColor = mPrimitiveShader.getUniform("uPrimitiveColor");
  mUniforms.primitiveColor = Luna::ColorRgb{1.0f, 0.f, 0.f, 1.f};

  mUniforms.uPrimitivePos = mPrimitiveShader.getUniform("uPrimitivePos");
  mUniforms.uPrimitivePos = info->position;

  CHECK_GL(glDrawArrays(
    GL_LINE_STRIP, 0, static_cast<int>(shape->getVertices().size())
  ));

  CHECK_GL(glDeleteBuffers(1, &pointBuffer));
  CHECK_GL(glDeleteVertexArrays(1, &vertexAttribConf));
}

void OpenglRenderer::setTextureFilterEnabled(
  [[maybe_unused]] int id, [[maybe_unused]] bool enabled
) {}

void OpenglRenderer::setRenderTargetTexture(int id) {
  GLuint framebuffer;

  if (mFramebuffers.count(id) == 0) {
    CHECK_GL(glGenFramebuffers(1, &framebuffer));
    mFramebuffers.emplace(id, framebuffer);
  } else {
    framebuffer = mFramebuffers.at(id);
  }

  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

  GLuint texture = mTextureIdMapping.at(id);
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
  CHECK_GL(glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0
  ));

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    logWarn("framebuffer is not complete");
  }

  mUsingFramebuffer = true;
}

void OpenglRenderer::unsetRenderTargetTexture() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  mUsingFramebuffer = false;
}

void OpenglRenderer::setViewport(Vector2i offset, Vector2i size) {
  CHECK_GL(glViewport(offset.x, offset.y, size.width, size.height));
}

void OpenglRenderer::imguiNewFrame() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplOpenGL3_NewFrame();
#ifdef LUNA_WINDOW_SDL2
    ImGui_ImplSDL2_NewFrame();
#endif
    ImGui::NewFrame();
  }
#endif
}

#endif
