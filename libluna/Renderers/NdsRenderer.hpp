#include <libluna/AbstractRenderer.hpp>

#include <memory>

namespace Luna {
  class NdsRenderer : public AbstractRenderer {
    public:
    NdsRenderer();
    ~NdsRenderer() override;

    void initialize() override;
    void initializeImmediateGui() override;
    void close() override;
    void render() override;
    void present() override;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
