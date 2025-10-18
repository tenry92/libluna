#pragma once

#include <libluna/Input/N64Gamepad.hpp>
#include <libluna/Input/N64GamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class N64GamepadDevice {
    public:
    N64GamepadDevice(const N64GamepadState* state);

    bool isDown(N64Gamepad::Button button) const;
    float getAxis(N64Gamepad::Axis axis) const;
    Vector2f getStick() const;

    private:
    const N64GamepadState* mState;
  };
}
