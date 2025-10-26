#pragma once

#include <forward_list>
#include <set>
#include <variant>
#include <vector>

#include <libluna/AbstractRenderer.hpp>
#include <libluna/Font.hpp>
#include <libluna/IdAllocator.hpp>
#include <libluna/Matrix.hpp>
#include <libluna/Mesh.hpp>
#include <libluna/Rect.hpp>
#include <libluna/Shape.hpp>
#include <libluna/Stage.hpp>
#include <libluna/Texture.hpp>

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
    struct GpuSubTexture {
      uint16_t id; ///< The internal texture ID.
      Recti crop;
    };

    struct GpuTexture {
      uint16_t id; ///< The internal texture ID. 0 if it uses sub textures.
      Vector2i size; ///< The size of the texture in pixels.
      std::vector<GpuSubTexture> subTextures;
    };

    /**
     * @brief This holds information about how to render a 2D texture.
     */
    struct RenderTextureInfo {
      const GpuTexture* gpuTexture;

      const GpuSubTexture* gpuSubTexture;

      /**
       * @brief The internal texture ID to render.
       */
      uint16_t textureId;

      /**
       * @brief The crop rectangle in pixels. If empty, the whole texture is used.
       */
      Recti crop;

      /**
       * @brief The output position. Origin is the top left corner.
       */
      Vector2f position;

      /**
       * @brief The output size in pixels.
       */
      Vector2i size;
    };

    struct RenderShapeInfo {
      int shapeId;

      Vector2f position;
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
      uint16_t diffuseTextureId{0};

      /**
       * @brief The normal texture ID to apply or 0 if none.
       */
      uint16_t normalTextureId{0};

      /**
       * @brief The local transform to apply to the mesh.
       */
      Matrix4x4 transform;
    };

    CommonRenderer();
    virtual ~CommonRenderer() override;

    void render() override;

    /**
     * @brief Declare a texture in the GPU texture mapping.
     *
     * @p gpuTexture must be prepared with the texture and sub texture sizes.
     * Their IDs will be set in this method.
     *
     * @param slot The texture slot.
     * @param gpuTexture The GPU texture information.
     */
    void declareGpuTexture(int slot, GpuTexture& gpuTexture);

    GpuTexture* getGpuTexture(int slot);

    void freeGpuTexture(int slot);

    virtual void startRender();

    virtual void endRender();

    virtual void clearBackground(ColorRgb color);

    virtual void createFramebufferTexture(uint16_t id, Vector2i size) = 0;

    virtual void resizeFramebufferTexture(uint16_t id, Vector2i size) = 0;

    virtual void destroyFramebufferTexture(uint16_t id) = 0;

    /**
     * @brief Slice a texture before loading to GPU.
     *
     * Returns true if the texture was sliced, false otherwise.
     */
    virtual bool sliceTexture(
      Texture* texture, std::vector<Texture>& slices, Vector2i& sliceCount
    );

    /**
     * @brief Draw a texture on the canvas.
     */
    virtual void renderTexture(Canvas* canvas, RenderTextureInfo* info);

    virtual void createShape(int id);

    virtual void destroyShape(int id);

    virtual void loadShape(int id, Shape* shape);

    virtual void renderShape(Canvas* canvas, RenderShapeInfo* info);

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
    virtual void renderMesh(Canvas* canvas, RenderMeshInfo* info);

    virtual void setTextureFilterEnabled(uint16_t id, bool enabled);
    virtual void setRenderTargetTexture(uint16_t id);
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

    Vector2i getTextureSize(int slot) const;

    private:
    /**
     * @brief Render all 3D mesh on the canvas.
     *
     * This is usually done before anything 2D is rendered.
     */
    void renderWorld(Canvas* canvas);

    /**
     * @brief Render all 2D drawables on the canvas.
     *
     * For scaling and interpolation reasons, this is usually done onto a render
     * texture and later drawn onto the canvas.
     */
    void render2d(Canvas* canvas, Vector2i renderSize);

    void start2dFramebuffer(Canvas* canvas);

    void end2dFramebuffer(Canvas* canvas);

    IdAllocator<uint16_t> mTextureIdAllocator;
    uint16_t mRenderTargetId;
    Vector2i mCurrentRenderSize;
    std::map<int, GpuTexture> mGpuTextureSlotMapping;
    std::unordered_map<Shape*, int> mKnownShapes;
    std::set<FontPtr> mLoadedFonts;
    std::unordered_map<std::shared_ptr<Mesh>, int> mKnownMeshes;
  };
} // namespace Luna
