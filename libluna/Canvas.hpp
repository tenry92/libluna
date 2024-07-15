#pragma once

#include <memory>
#include <queue>

#include <libluna/ButtonEvent.hpp>
#include <libluna/Camera2d.hpp>
#include <libluna/Camera3d.hpp>
#include <libluna/Color.hpp>
#include <libluna/Image.hpp>
#include <libluna/ImmediateGui.hpp>
#include <libluna/Stage.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class CanvasImpl;

  /**
   * @brief Abstraction for windows (computer) and fullscreen displays.
   */
  class Canvas {
    public:
    Canvas(const Vector2i &size);
    ~Canvas();

    void close();

    void setVideoDriver(const String &name);

    void attachImmediateGui(std::unique_ptr<ImmediateGui> gui);
    ImmediateGui *getImmediateGui() const;

    void setStage(std::shared_ptr<Stage> stage);
    std::shared_ptr<Stage> getStage() const;

    void setCamera2d(const Camera2d &camera);
    Camera2d getCamera2d() const;

    void setCamera3d(const Camera3d &camera);
    Camera3d getCamera3d() const;

    void setBackgroundColor(ColorRgb color);
    ColorRgb getBackgroundColor() const;

    ImagePtr captureScreenshot();

    void render();
    void sync();

    std::queue<ButtonEvent> &getButtonEvents() const;

    /**
     * @brief Get the current value of an axis input.
     *
     * The axis may be the horizontal or vertical position of a joystick or the
     * value of a trigger button.
     */
    float getInputAxis(const String &name);

    bool isClosed() const;

    CanvasImpl *getImpl() const;

    private:
    std::unique_ptr<CanvasImpl> mImpl;
  };
} // namespace Luna
