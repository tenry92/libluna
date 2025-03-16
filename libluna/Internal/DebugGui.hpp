#pragma once

#include <libluna/ImmediateGui.hpp>
#include <libluna/Internal/DebugMetrics.hpp>

namespace Luna::Internal {
  class DebugGui : public Luna::ImmediateGui {
    public:
    DebugGui(std::shared_ptr<DebugMetrics> metrics);

    protected:
    void render() override;

    private:
    std::shared_ptr<DebugMetrics> mMetrics;
    bool mShowDemoWindow{false};
    bool mShowLunaDebugger{false};
  };
} // namespace Luna::Internal
