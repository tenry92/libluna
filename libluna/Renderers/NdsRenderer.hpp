#pragma once

#include <libluna/AbstractRenderer.hpp>

namespace Luna {
  class NdsRenderer : public AbstractRenderer {
    public:
    NdsRenderer();
    ~NdsRenderer();

    void initialize() override;
    void initializeImmediateGui() override;
    void quitImmediateGui() override;
    void close() override;
    void render() override;
    void present() override;

    Internal::GraphicsMetrics getMetrics() override;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
}
