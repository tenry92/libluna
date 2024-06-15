#pragma once

#include <memory>

#include <libluna/AbstractRenderer.hpp>
#include <libluna/Matrix.hpp>

namespace Luna {
  /**
   * @brief Base class for modern renderers.
   *
   * This class takes care of managing textures, drawing calls etc.
   * Any sub class can focus on the actual implementation and doesn't need to
   * care about drawing the sprites in the right order etc.
   *
   * This class might not be suitable for certain specialized backends, for
   * example if the system renders sprites automatically and doesn't allow for
   * explicit draw commands.
   */
  class CommonRenderer : public AbstractRenderer {
    public:
    /**
     * @brief This holds information about how to render a 2D texture.
     */
    struct RenderTextureInfo {
      /**
       * @brief The texture ID to render.
       */
      int textureId;

      /**
       * @brief The output position. Origin is the top left corner.
       */
      Vector2f position;

      /**
       * @brief The output size in pixels.
       */
      Vector2i size;
    };

    /**
     * @brief This holds information about how to render a 3D mesh.
     */
    struct RenderMeshInfo {
      /**
       * @brief The mesh ID to render.
       */
      int meshId;

      /**
       * @brief The diffuse texture ID to apply or 0 if none.
       */
      int diffuseTextureId{0};

      /**
       * @brief The normal texture ID to apply or 0 if none.
       */
      int normalTextureId{0};

      /**
       * @brief The local transform to apply to the mesh.
       */
      Matrix4x4 transform;
    };

    CommonRenderer();
    virtual ~CommonRenderer() override;

    void render() override;

    virtual void clearBackground(Color color);

    /**
     * @brief Create a new empty texture for the provided ID.
     * 
     * This may do nothing and the actual creation can be done later in
     * @ref loadTexture().
     */
    virtual void createTexture(int id);

    /**
     * @brief Destroy a texture for the provided ID.
     */
    virtual void destroyTexture(int id);

    /**
     * @brief Load data to the given texture ID.
     * 
     * It is guaranteed that the given texture ID was previously passed to
     * @ref createTexture().
     */
    virtual void loadTexture(int id, std::shared_ptr<Image> image, int frameIndex);

    /**
     * @brief Resize the texture for the provided ID.
     */
    virtual void resizeTexture(int id, Vector2i size);

    /**
     * @brief Draw a texture on the canvas.
     */
    virtual void renderTexture(Canvas *canvas, RenderTextureInfo *info);

    /**
     * @brief Create a new empty mesh for the provided ID.
     *
     * This may do nothing and the actual creation can be done later in
     * @ref loadMesh().
     */
    virtual void createMesh(int id);

    /**
     * @brief Destroy a mesh for the provided ID.
     */
    virtual void destroyMesh(int id);

    /**
     * @brief Load data to the given mesh ID.
     *
     * It is guaranteed that the given mesh ID was previously passed to
     * @ref createMesh().
     */
    virtual void loadMesh(int id, std::shared_ptr<Mesh> mesh);

    /**
     * @brief Draw a 3D mesh on the canvas.
     */
    virtual void renderMesh(Canvas *canvas, RenderMeshInfo *info);

    virtual void setTextureFilterEnabled(int id, bool enabled);
    virtual void setRenderTargetTexture(int id);
    virtual void unsetRenderTargetTexture();

    virtual void setViewport(Vector2i offset, Vector2i size);

    virtual void imguiNewFrame();

    /**
     * @brief Get the actual canvas size.
     *
     * This should normally not be used since its probable we are currently
     * rendering to a framebuffer of a different dimension.
     */
    Vector2i getCanvasSize() const;

    /**
     * @brief Get the internal (original) canvas size.
     *
     * This should normally not be used since its probable we are currently
     * rendering to a framebuffer of a different dimension.
     */
    Vector2i getInternalSize() const;

    Vector2i getCurrentRenderSize() const;

    private:
    /**
     * @brief Render all 3D mesh on the canvas.
     *
     * This is usually done before anything 2D is rendered.
     */
    void renderWorld(Canvas *canvas);

    /**
     * @brief Render all 2D sprites on the canvas.
     *
     * For scaling and interpolation reasons, this is usually done onto a render
     * texture and later drawn onto the canvas.
     */
    void renderSprites(Canvas *canvas, Vector2i renderSize);

    void updateTextureCache(std::shared_ptr<Stage> stage);

    void start2dFramebuffer(Canvas *canvas);

    void end2dFramebuffer(Canvas *canvas);

    int mNextTextureId;
    int mNextMeshId;
    int mRenderTargetId;
    Vector2i mCurrentRenderSize;
    std::unordered_map<std::shared_ptr<ResourceRef<Image>>, int> mKnownImages;
    std::unordered_map<std::shared_ptr<ResourceRef<Image>>, Vector2i> mImageSizes;
    std::unordered_map<std::shared_ptr<Mesh>, int> mKnownMeshes;
  };
} // namespace Luna
