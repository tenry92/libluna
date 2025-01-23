#include <libluna/config.h>

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#endif

#include <chrono>

#include <libluna/Application.hpp>
#include <libluna/Internal/DebugGui.hpp>
#include <libluna/Platform.hpp>
#include <libluna/System.hpp>

using namespace Luna;
using namespace Luna::Audio;
using namespace Luna::Internal;

DebugGui::DebugGui(std::shared_ptr<DebugMetrics> metrics) : mMetrics{metrics} {}

void DebugGui::render() {
  ImGui::ShowDemoWindow();

  if (ImGui::Begin("Debugger", nullptr, 0)) {
    // todo: lock audio device before reading metrics
    auto audioMetrics = &AudioManager::getInstance()->getMetrics();

    if (Platform::isDebug()) {
      ImGui::Text("DEBUG BUILD");
    } else {
      ImGui::Text("RELEASE BUILD");
    }

    ImGui::Text("App version: " VERSION_STRING);

    ImGui::Separator();

    ImGui::Text("Frames elapsed: %d", mMetrics->framesElapsed);
    ImGui::Text(
      "Average tick duration: %.2fms",
      mMetrics->frameTicker.getTickDuration() * std::milli::den
    );
    ImGui::Text(
      "Ticks per second: %0.1f", mMetrics->frameTicker.getTicksPerSecond()
    );
    ImGui::Text(
      "Average render duration: %.2fms",
      mMetrics->renderTicker.getTickDuration() * std::milli::den
    );
    ImGui::Text(
      "Renders per second: %0.1f", mMetrics->renderTicker.getTicksPerSecond()
    );

    ImGui::Separator();

    float audioLoad =
      audioMetrics->renderTicker.getTickDuration() / audioMetrics->bufferSize;
    ImGui::Text(
      "Audio buffer size: %.2fms", audioMetrics->bufferSize * std::milli::den
    );
    ImGui::Text(
      "Audio render duration: %.2fms (%.1f%%)",
      audioMetrics->renderTicker.getTickDuration() * std::milli::den,
      audioLoad * 100.0f
    );

    ImGui::Separator();

    auto app = Application::getInstance();
    ImGui::Text("Processor count: %d", System::getProcessorCount());
    ImGui::Text(
      "Physical memory usage: %.1f MiB",
      static_cast<float>(System::getPhysicalMemoryUsage() / 1024 / 1024)
    );
    ImGui::Text(
      "Virtual memory usage: %.1f MiB",
      static_cast<float>(System::getVirtualMemoryUsage() / 1024 / 1024)
    );

    ImGui::Separator();

    auto canvases = app->getOpenCanvases();
    ImGui::Text("Open canvases: %lu", canvases.size());

    for (auto &&canvas : canvases) {
      if (ImGui::BeginTabBar("CanvasTabBar")) {
        auto metrics = canvas->getMetrics();

        if (ImGui::BeginTabItem("Graphics")) {
          ImGui::Text(
            "Max OpenGL version: %d.%d", metrics.glMajor, metrics.glMinor
          );
          ImGui::Text("Vendor: %s", metrics.vendor.c_str());
          ImGui::Text(
            "Shading lang version: %s", metrics.shadingLangVersion.c_str()
          );
          ImGui::Text("Max texture size: %d²", metrics.maxTextureSize);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sprites")) {
          ImGui::Text("Sprites: %d", metrics.spriteCount);
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Textures")) {
          ImGui::Text("Textures: %d", metrics.textureCount);
          ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
      }
    }
    // event queue, axis values
    // renderer, textures, resources
  }
  ImGui::End();
}
