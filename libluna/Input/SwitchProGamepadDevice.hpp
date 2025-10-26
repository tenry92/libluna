#pragma once

#include <libluna/Input/SwitchProGamepad.hpp>
#include <libluna/Input/SwitchProGamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  /**
   * @brief Represents a Nintendo Switch Pro gamepad input device.
   *
   * @ingroup input input_devices
   */
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
