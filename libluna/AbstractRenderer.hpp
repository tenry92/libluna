#pragma once

#include <memory>

#include <libluna/Canvas.hpp>
#include <libluna/Internal/GraphicsMetrics.hpp>

namespace Luna {
  class AbstractRenderer {
    public:
    AbstractRenderer();
    virtual ~AbstractRenderer();

    void setCanvas(Canvas *canvas);
    Canvas *getCanvas() const;

    virtual void initialize() = 0;
    virtual void initializeImmediateGui() = 0;
    virtual void quitImmediateGui() = 0;
    virtual void close() = 0;
    virtual void render() = 0;
    virtual void present() = 0;

    virtual Internal::GraphicsMetrics getMetrics() = 0;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
