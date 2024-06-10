#include <libluna/config.h>

#ifdef LUNA_USE_OPENGL
#include <libluna/Renderers/OpenglRenderer.hpp>

#include <list>
#include <map>

#ifdef LUNA_USE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_USE_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#include <glad/glad.h>

#ifdef LUNA_USE_IMGUI
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#ifdef LUNA_USE_SDL
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#endif

#ifdef LUNA_USE_GLWF
#include <imgui/backends/imgui_impl_glfw.h>
#endif
#endif // IMGUI

#include <libluna/Application.hpp>
#include <libluna/Logger.hpp>
#include <libluna/MemoryReader.hpp>
#include <libluna/TextureCache.hpp>

#include <libluna/CanvasImpl.hpp>

#include <libluna/GL/Shader.hpp>
#include <libluna/GL/ShaderLib.hpp>
#include <libluna/GL/Uniform.hpp>

#include <libluna/GL/shaders/3d_frag.glsl.h>
#include <libluna/GL/shaders/3d_vert.glsl.h>
#include <libluna/GL/shaders/common3d.glsl.h>
#include <libluna/GL/shaders/sprite_frag.glsl.h>
#include <libluna/GL/shaders/sprite_vert.glsl.h>

#define CHECK_GL(x)                                                            \
  {                                                                            \
    glGetError();                                                              \
    GLenum err;                                                                \
    x;                                                                         \
    while ((err = glGetError()) != GL_NO_ERROR)                                \
      logError("opengl error [" #x "]: {}", getGlErrorString(err));            \
  }

using namespace Luna;

class GlTextureWrapper {
  public:
  GlTextureWrapper(
      GLuint texture, int width, int height,
      std::shared_ptr<Internal::GraphicsMetrics> metrics
  )
      : mTexture{texture}, mWidth{width}, mHeight{height}, mMetrics{metrics} {
    ++mMetrics->textureCount;
  }

  ~GlTextureWrapper() {
    glDeleteTextures(1, &mTexture);
    --mMetrics->textureCount;
  }

  GLuint getTexture() const { return mTexture; }

  int getWidth() const { return mWidth; }

  int getHeight() const { return mHeight; }

  private:
  GLuint mTexture;
  int mWidth;
  int mHeight;
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;
};

using TextureType = std::shared_ptr<GlTextureWrapper>;
using TextureCacheType = TextureCache<TextureType>;

const char *getGlErrorString(GLenum err) {
  switch (err) {
  default:
    return "unknown";
  case GL_INVALID_ENUM:
    return "invalid enum";
  case GL_INVALID_VALUE:
    return "invalid value";
  case GL_INVALID_OPERATION:
    return "invalid operation";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "invalid framebuffer operation";
  case GL_OUT_OF_MEMORY:
    return "out of memory";
  }
}

class OpenglRenderer::impl {
  public:
  void clearBackground(Color color = {1.0f, 0.5f, 1.0f, 1.0f}) {
    CHECK_GL(glClearColor(color.red, color.green, color.blue, 1.0f));
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  }

  void updateTextureCache(Canvas *canvas) {
    if (!mTextureCache || mTextureCache->getStage() != canvas->getStage()) {
      std::function<TextureType(std::shared_ptr<Image>, int)> callback =
          [this](std::shared_ptr<Image> image, int frameIndex) -> TextureType {
        logDebug("loading gl texture from image#{}", frameIndex);

        if (!image->isTrue()) {
          /// @todo Get palette from sprite
          image = std::make_shared<Image>(image->toTrue(nullptr));
        }

        GLuint texture;
        CHECK_GL(glGenTextures(1, &texture));
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
        CHECK_GL(glTexImage2D(
            GL_TEXTURE_2D, 0,                              /* mipmap level */
            GL_RGBA,                                       /* internal format */
            image->getSize().x(), image->getSize().y(), 0, /* format (legacy) */
            GL_RGBA,                                       /* input format */
            GL_UNSIGNED_BYTE, image->getFrameData(frameIndex)
        ));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return std::make_shared<GlTextureWrapper>(
            texture, image->getSize().x(), image->getSize().y(), mMetrics
        );
      };
      mTextureCache =
          std::make_unique<TextureCacheType>(canvas->getStage(), callback);
    }

    mTextureCache->updateCache();
  }

  void renderSprites(Canvas *canvas, int width, int height) {
    mSpriteShader.use();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);
    mUniforms.screenSize = mSpriteShader.getUniform("uScreenSize");
    mUniforms.screenSize =
        Vector2f(static_cast<float>(width), static_cast<float>(height));

    for (auto &&sprite : canvas->getStage()->getSprites()) {
      renderSprite(canvas, sprite);
    }
  }

  void renderSprite(Canvas *canvas, std::shared_ptr<Sprite> sprite) {
    if (!mTextureCache->hasTextureBySprite(sprite)) {
      return;
    }

    auto texture = mTextureCache->getTextureBySprite(sprite);

    float vertices[] = {/* x, y, u, v */
                        0.f,
                        0.f,
                        0.f,
                        0.f,
                        static_cast<float>(texture->getWidth()),
                        0.f,
                        1.f,
                        0.f,
                        static_cast<float>(texture->getWidth()),
                        static_cast<float>(texture->getHeight()),
                        1.f,
                        1.f,
                        0.f,
                        static_cast<float>(texture->getHeight()),
                        0.f,
                        1.f};

    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    // vbo: store the actual vertex buffer (positions, coordinates etc.)
    unsigned int vbo;
    CHECK_GL(glGenBuffers(1, &vbo));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CHECK_GL(glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW
    ));

    // vao: store vertex attrib configurations
    unsigned int vao;
    CHECK_GL(glGenVertexArrays(1, &vao));
    CHECK_GL(glBindVertexArray(vao));
    // data layout:
    // [V1  ] [V2  ] [V3  ] [V4  ]
    // [x, y] [x, y] [x, y] [x, y]
    CHECK_GL(glVertexAttribPointer(
        0,        // input location = 0
        2,        // vec2
        GL_FLOAT, // vector of floats
        GL_FALSE, // don't normalize input data
        4 * sizeof(float
            ),    // stride; offset between each vector (2 = xy, 4 = xyvu)
        (void *)0 // offset; where data begins in the buffer
    ));
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glVertexAttribPointer(
        1,        // input location = 1
        2,        // vec2
        GL_FLOAT, // vector of floats
        GL_FALSE, // don't normalize input data
        4 * sizeof(float
            ), // stride; offset between each vector (2 = xy, 4 = xyvu)
        (void *)(2 * sizeof(float)) // offset; where data begins in the buffer
    ));
    CHECK_GL(glEnableVertexAttribArray(1));

    // drawing using element buffer instead:
    unsigned int ebo;
    CHECK_GL(glGenBuffers(1, &ebo));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    CHECK_GL(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW
    ));

    mUniforms.spriteTexture = mSpriteShader.getUniform("uSpriteTexture");
    mUniforms.spriteTexture = 0;
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture->getTexture()));

    auto transformedPosition =
        sprite->getPosition() - canvas->getCamera2d().getPosition();
    mUniforms.spritePos = mSpriteShader.getUniform("uSpritePos");
    mUniforms.spritePos = transformedPosition;

    CHECK_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

    CHECK_GL(glDeleteBuffers(1, &ebo));
    CHECK_GL(glDeleteVertexArrays(1, &vao));
    CHECK_GL(glDeleteBuffers(1, &vbo));
  }

  void renderWorld(Canvas *canvas) {
    mModelShader.use();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    auto ambientLight = canvas->getStage()->getAmbientLight();
    mUniforms.ambientLightColor =
        mModelShader.getUniform("uAmbientLight.color");
    mUniforms.ambientLightIntensity =
        mModelShader.getUniform("uAmbientLight.intensity");
    mUniforms.ambientLightColor = ambientLight.color;
    mUniforms.ambientLightIntensity = ambientLight.intensity;

    mUniforms.pointLightsColor =
        mModelShader.getUniform("uPointLights[{}].color", 1);
    mUniforms.pointLightsPosition =
        mModelShader.getUniform("uPointLights[{}].position", 1);

    // int lightIndex = 0;

    for (auto &&pointLight : canvas->getStage()->getPointLights()) {
      mUniforms.pointLightsColor[0] = pointLight->color;
      mUniforms.pointLightsPosition[0] = pointLight->position;
    }

    for (auto &&model : canvas->getStage()->getModels()) {
      renderModel(canvas, model);
    }
  }

  void renderModel(Canvas *canvas, std::shared_ptr<Model> model) {
    auto mesh = model->getMesh();

    if (!mesh) {
      return;
    }

    if (!mTextureCache->hasTextureByImage(model->getMaterial().getDiffuse())) {
      return;
    }

    auto texture =
        mTextureCache->getTextureByImage(model->getMaterial().getDiffuse());

    std::shared_ptr<GlTextureWrapper> normalMap;

    if (mTextureCache->hasTextureByImage(model->getMaterial().getNormal())) {
      normalMap =
          mTextureCache->getTextureByImage(model->getMaterial().getNormal());
    }

    // vertices: [[x, y, z, u, v, nx, ny, nz], ...]
    std::vector<float> vertices;
    vertices.reserve(mesh->getVertices().size() * 14);

    for (int i = 0; i < static_cast<int>(mesh->getVertices().size()); ++i) {
      auto &&vertex = mesh->getVertices().at(i);
      vertices.push_back(vertex.x());
      vertices.push_back(vertex.y());
      vertices.push_back(vertex.z());

      if (i < static_cast<int>(mesh->getTexCoords().size())) {
        auto &&texCoords = mesh->getTexCoords().at(i);
        vertices.push_back(texCoords.x()); // u
        vertices.push_back(texCoords.y()); // v
      } else {
        vertices.push_back(0.0f); // u
        vertices.push_back(0.0f); // v
      }

      if (i < static_cast<int>(mesh->getNormals().size())) {
        auto &&normal = mesh->getNormals().at(i);
        vertices.push_back(normal.x());
        vertices.push_back(normal.y());
        vertices.push_back(normal.z());
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }

      if (i < static_cast<int>(mesh->getTangents().size())) {
        auto &&tangent = mesh->getTangents().at(i);
        vertices.push_back(tangent.x());
        vertices.push_back(tangent.y());
        vertices.push_back(tangent.z());
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }

      if (i < static_cast<int>(mesh->getBitangents().size())) {
        auto &&bitangent = mesh->getBitangents().at(i);
        vertices.push_back(bitangent.x());
        vertices.push_back(bitangent.y());
        vertices.push_back(bitangent.z());
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }
    }

    std::vector<unsigned int> indices;
    indices.reserve(mesh->getFaces().size() * 3);

    for (auto &&face : mesh->getFaces()) {
      indices.push_back(static_cast<unsigned int>(face.at(0)));
      indices.push_back(static_cast<unsigned int>(face.at(1)));
      indices.push_back(static_cast<unsigned int>(face.at(2)));
    }

    // vbo: store the actual vertex buffer (positions, coordinates etc.)
    unsigned int vbo;
    CHECK_GL(glGenBuffers(1, &vbo));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CHECK_GL(glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
        GL_STATIC_DRAW
    ));

    // vao: store vertex attrib configurations
    unsigned int vao;
    CHECK_GL(glGenVertexArrays(1, &vao));
    CHECK_GL(glBindVertexArray(vao));
    int stride = 14 * sizeof(float);

    CHECK_GL(glVertexAttribPointer(
        0,        // input location = 0
        3,        // vec3
        GL_FLOAT, // vector of floats
        GL_FALSE, // don't normalize input data
        stride,   // stride; offset between each vector
        (void *)0 // offset; where data begins in the buffer
    ));
    CHECK_GL(glEnableVertexAttribArray(0));

    CHECK_GL(glVertexAttribPointer(
        1,        // input location = 1
        2,        // vec2
        GL_FLOAT, // vector of floats
        GL_FALSE, // don't normalize input data
        stride,   // stride; offset between each vector (2 = xy, 4 = xyvu)
        (void *)(3 * sizeof(float)) // offset; where data begins in the buffer
    ));
    CHECK_GL(glEnableVertexAttribArray(1));

    CHECK_GL(glVertexAttribPointer(
        2,        // input location = 2
        3,        // vec3
        GL_FLOAT, // vector of floats
        GL_TRUE,  // normalize input data
        stride,   // stride; offset between each vector (2 = xy, 4 = xyvu)
        (void *)(5 * sizeof(float)) // offset; where data begins in the buffer
    ));
    CHECK_GL(glEnableVertexAttribArray(2));

    // GL_TRIANGLES: {0 1 2} {3 4 5}
    // GL_TRIANGLE_STRIP: {0 1 2} {1 2 3}
    // GL_TRIANGLE_FAN: {0 1 2} {0 2 3}
    // CHECK_GL(glDrawArrays(GL_TRIANGLES, 0, 3));

    // drawing using element buffer instead:
    unsigned int ebo;
    CHECK_GL(glGenBuffers(1, &ebo));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    CHECK_GL(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW
    ));

    mUniforms.materialDiffuseMap =
        mModelShader.getUniform("uMaterial.diffuseMap");
    mUniforms.materialDiffuseMap = 0;
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture->getTexture()));

    if (normalMap) {
      mUniforms.materialNormalMap =
          mModelShader.getUniform("uMaterial.normalMap");
      mUniforms.materialNormalMap = 1;
      CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture->getTexture()));

      glActiveTexture(GL_TEXTURE1);
      CHECK_GL(glBindTexture(GL_TEXTURE_2D, normalMap->getTexture()));
      glActiveTexture(GL_TEXTURE0);
    }

    mUniforms.transformModel = mModelShader.getUniform("uTransform.model");
    mUniforms.transformModel = model->getTransform();

    auto camera = canvas->getCamera3d();

    mUniforms.transformView = mModelShader.getUniform("uTransform.view");
    mUniforms.transformView = camera.getViewMatrix();

    mUniforms.transformProjection =
        mModelShader.getUniform("uTransform.projection");
    mUniforms.transformProjection = camera.getProjectionMatrix(4.0f / 3.0f);

    mUniforms.viewPos = mModelShader.getUniform("uViewPos");
    mUniforms.viewPos = camera.getPosition();

    CHECK_GL(glDrawElements(
        GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0
    ));

    CHECK_GL(glDeleteBuffers(1, &ebo));
    CHECK_GL(glDeleteVertexArrays(1, &vao));
    CHECK_GL(glDeleteBuffers(1, &vbo));
  }

#ifdef LUNA_USE_IMGUI
  ImGuiContext *mImGuiContext{nullptr};
#endif
  GL::Shader mSpriteShader;
  GL::Shader mModelShader;
  std::unique_ptr<TextureCacheType> mTextureCache;
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
#ifdef LUNA_USE_IMGUI
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
#ifdef LUNA_USE_IMGUI
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
#ifdef LUNA_USE_IMGUI
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

void OpenglRenderer::render() {
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplOpenGL3_NewFrame();
#ifdef LUNA_USE_SDL
    ImGui_ImplSDL2_NewFrame();
#endif
    ImGui::NewFrame();
  }
#endif

  int width = 800;
  int height = 600;

#ifdef LUNA_USE_SDL
  SDL_GetWindowSize(getCanvas()->getImpl()->sdl.window, &width, &height);
#endif
#ifdef LUNA_USE_GLFW
  glfwGetFramebufferSize(getCanvas()->getImpl()->glfw.window, &width, &height);
#endif

  CHECK_GL(glViewport(0, 0, width, height));

  auto canvas = getCanvas();

  if (!canvas) {
    mImpl->clearBackground();

    return;
  }

  auto bgColor = canvas->getBackgroundColor();

  mImpl->clearBackground(bgColor);

  if (!canvas->getStage()) {
    return;
  }

  mImpl->updateTextureCache(canvas);
  mImpl->renderWorld(canvas);
  mImpl->renderSprites(canvas, width, height);
}

void OpenglRenderer::present() {
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
#endif

#ifdef LUNA_USE_SDL
  SDL_GL_SwapWindow(getCanvas()->getImpl()->sdl.window);
#endif

#ifdef LUNA_USE_GLFW
  glfwSwapBuffers(getCanvas()->getImpl()->glfw.window);
#endif
}

Internal::GraphicsMetrics OpenglRenderer::getMetrics() {
  return *mImpl->mMetrics;
}

#endif
