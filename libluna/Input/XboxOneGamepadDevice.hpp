#pragma once

#include <libluna/Input/XboxOneGamepad.hpp>
#include <libluna/Input/XboxOneGamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class XboxOneGamepadDevice {
    public:
    XboxOneGamepadDevice(const XboxOneGamepadState* state);

    bool isDown(XboxOneGamepad::Button button) const;
    float getAxis(XboxOneGamepad::Axis axis) const;
    Vector2f getStick(XboxOneGamepad::Stick stick) const;

    private:
    const XboxOneGamepadState* mState;
  };
}
