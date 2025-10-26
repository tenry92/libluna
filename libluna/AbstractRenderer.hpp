#pragma once

#include <libluna/Internal/GraphicsMetrics.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Canvas;

  /**
   * @defgroup renderers Renderers
   *
   * @brief Low-level implementation of various renderers.
   */

  /**
   * @brief Base class for rendering onto a canvas.
   *
   * @ingroup renderers
   */
  class AbstractRenderer {
    public:
    AbstractRenderer();
    virtual ~AbstractRenderer();

    /**
     * @brief Associate a canvas where to draw to.
     */
    void setCanvas(Canvas* canvas);

    /**
     * @brief Get associated canvas.
     */
    Canvas* getCanvas() const;

    /**
     * @brief Called once to initialize the renderer.
     *
     * The implementation should initialize the underlying rendering system and
     * load required resources (e.g., shaders).
     *
     * @see initializeImmediateGui()
     */
    virtual void initialize() = 0;

    /**
     * @brief Called once after @ref initialize() to initialize the immediate gui.
     *
     * @see initialize()
     */
    virtual void initializeImmediateGui() = 0;

    virtual void quitImmediateGui() = 0;

    /**
     * @brief Called once when closing the related canvas.
     */
    virtual void close() = 0;

    /**
     * @brief Called once per frame to initiate rendering.
     *
     * The implementation should clear the background and render every camera
     * associated with the canvas.
     *
     * The @ref CommonRenderer provides a good abstraction, which can be used by
     * renderers on modern platforms which easily allow arbitrary drawing of
     * tetures and 3D meshes.
     *
     * @see present()
     */
    virtual void render() = 0;

    /**
     * @brief Called once per frame after @ref render() to display the rendered frame.
     *
     * This may sync with the screen (VSync).
     *
     * @see render()
     */
    virtual void present() = 0;

    virtual Internal::GraphicsMetrics getMetrics() = 0;

    /**
     * @brief Upload texture data to the GPU for the given ID.
     *
     * If a texture is already present at the given slot, it is to be replaced.
     *
     * If the texture is too large for the underlying system, it may be sliced
     * into smaller textures under the hood.
     */
    virtual void uploadTexture(int slot, const Texture* texture) = 0;

    /**
     * @brief Free the GPU resources associated with the texture at the given slot.
     *
     * @see uploadTexture()
     */
    virtual void freeTexture(int slot) = 0;

    private:
    Canvas* mCanvas;
  };
} // namespace Luna
