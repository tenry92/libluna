#include <libluna/AbstractRenderer.hpp>

#include <memory>

namespace Luna {
  class OpenglRenderer : public AbstractRenderer {
    public:
    OpenglRenderer();
    ~OpenglRenderer() override;

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
} // namespace Luna
