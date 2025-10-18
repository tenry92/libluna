#pragma once

#include <libluna/Input/SwitchProGamepad.hpp>
#include <libluna/Input/SwitchProGamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class SwitchProGamepadDevice {
    public:
    SwitchProGamepadDevice(const SwitchProGamepadState* state);

    bool isDown(SwitchProGamepad::Button button) const;
    float getAxis(SwitchProGamepad::Axis axis) const;
    Vector2f getStick(SwitchProGamepad::Stick stick) const;

    private:
    const SwitchProGamepadState* mState;
  };
}
