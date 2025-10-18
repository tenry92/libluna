#pragma once

#include <libluna/Input/Xbox360Gamepad.hpp>
#include <libluna/Input/Xbox360GamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class Xbox360GamepadDevice {
    public:
    Xbox360GamepadDevice(const Xbox360GamepadState* state);

    bool isDown(Xbox360Gamepad::Button button) const;
    float getAxis(Xbox360Gamepad::Axis axis) const;
    Vector2f getStick(Xbox360Gamepad::Stick stick) const;

    private:
    const Xbox360GamepadState* mState;
  };
}
