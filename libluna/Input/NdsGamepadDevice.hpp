#pragma once

#include <libluna/Input/NdsGamepad.hpp>
#include <libluna/Input/NdsGamepadState.hpp>

namespace Luna::Input {
  /**
   * @brief Represents a Nintendo DS gamepad input device.
   *
   * @ingroup input input_devices
   */
  class NdsGamepadDevice {
    public:
    NdsGamepadDevice(const NdsGamepadState* state);

    bool isDown(NdsGamepad::Button button) const;

    private:
    const NdsGamepadState* mState;
  };
}
