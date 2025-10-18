#pragma once

#include <libluna/Input/Ps5Gamepad.hpp>
#include <libluna/Input/Ps5GamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class Ps5GamepadDevice {
    public:
    Ps5GamepadDevice(const Ps5GamepadState* state);

    bool isDown(Ps5Gamepad::Button button) const;
    float getAxis(Ps5Gamepad::Axis axis) const;
    Vector2f getStick(Ps5Gamepad::Stick stick) const;

    private:
    const Ps5GamepadState* mState;
  };
}
