#pragma once

#include <libluna/Internal/GraphicsMetrics.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Canvas;

  /**
   * @brief Base class for rendering onto a canvas.
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
     */
    virtual void initialize() = 0;

    /**
     * @brief Called once after @ref initialize() to initialize the immediate gui.
     */
    virtual void initializeImmediateGui() = 0;

    virtual void quitImmediateGui() = 0;

    /**
     * @brief Called once when closing the related canvas.
     */
    virtual void close() = 0;

    /**
     * @brief Called once per frame to initiate rendering.
     */
    virtual void render() = 0;

    /**
     * @brief Called once per frame after @ref render() to display the rendered frame.
     *
     * This may sync with the screen (VSync).
     */
    virtual void present() = 0;

    virtual Internal::GraphicsMetrics getMetrics() = 0;

    /**
     * @brief Upload texture data to the GPU for the given ID.
     */
    virtual void uploadTexture(int slot, const Texture* texture) = 0;

    virtual void freeTexture(int slot) = 0;

    private:
    Canvas* mCanvas;
  };
} // namespace Luna
