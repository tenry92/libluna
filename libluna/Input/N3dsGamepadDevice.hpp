#pragma once

#include <libluna/Input/N3dsGamepad.hpp>
#include <libluna/Input/N3dsGamepadState.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  /**
   * @brief Represents a Nintendo 3DS gamepad input device.
   *
   * @ingroup input input_devices
   */
  class N3dsGamepadDevice {
    public:
    N3dsGamepadDevice(const N3dsGamepadState* state);

    bool isDown(N3dsGamepad::Button button) const;
    float getAxis(N3dsGamepad::Axis axis) const;
    Vector2f getStick() const;

    private:
    const N3dsGamepadState* mState;
  };
}
