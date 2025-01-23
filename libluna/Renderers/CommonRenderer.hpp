#pragma once

#include <forward_list>
#include <variant>
#include <vector>

#include <libluna/AbstractRenderer.hpp>
#include <libluna/Font.hpp>
#include <libluna/IdAllocator.hpp>
#include <libluna/ImageLoader.hpp>
#include <libluna/Matrix.hpp>
#include <libluna/Mesh.hpp>
#include <libluna/Rect.hpp>
#include <libluna/Shape.hpp>
#include <libluna/Stage.hpp>

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
    struct Texture {
      int id;
      Vector2i size;
    };

    struct SlicedTexture {
      Vector2i sliceCount;
      std::vector<Texture> slices;
    };

    using TextureOrSlices = std::variant<Texture, SlicedTexture>;

    /**
     * @brief This holds information about how to render a 2D texture.
     */
    struct RenderTextureInfo {
      /**
       * @brief The texture ID to render.
       */
      int textureId;

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

    virtual void startRender();

    virtual void endRender();

    virtual void clearBackground(ColorRgb color);

    /**
     * @brief Slice an image before loading as textures.
     *
     * Returns true if the image was sliced, false otherwise.
     */
    virtual bool sliceTexture(
      Image* image, std::vector<Image>& slices, Vector2i& sliceCount
    );

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
    virtual void loadTexture(int id, Image* image);

    /**
     * @brief Resize the texture for the provided ID.
     */
    virtual void resizeTexture(int id, Vector2i size);

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

    Vector2i getTextureSize(int id) const;

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

    void updateTextureCache(Stage* stage);

    void start2dFramebuffer(Canvas* canvas);

    void end2dFramebuffer(Canvas* canvas);

    IdAllocator<uint16_t> mTextureIdAllocator;
    IdAllocator<uint16_t> mShapeIdAllocator;
    IdAllocator<uint16_t> mMeshIdAllocator;
    int mRenderTargetId;
    Vector2i mCurrentRenderSize;
    std::unordered_map<ImageLoader*, TextureOrSlices> mKnownImages;
    std::map<int, Texture> mTextureIdMapping;
    std::unordered_map<Shape*, int> mKnownShapes;
    std::set<FontPtr> mLoadedFonts;
    std::unordered_map<std::shared_ptr<Mesh>, int> mKnownMeshes;
  };
} // namespace Luna
