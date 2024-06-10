#include <libluna/AbstractRenderer.hpp>

#include <memory>

namespace Luna {
  class SdlRenderer : public AbstractRenderer {
    public:
    SdlRenderer();
    ~SdlRenderer() override;

    void initialize() override;
    void initializeImmediateGui() override;
    void quitImmediateGui() override;
    void close() override;
    void render() override;
    void present() override;
    Internal::GraphicsMetrics getMetrics() override;

    private:
    void drawDebugGrid();
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
