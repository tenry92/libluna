#pragma once

#include <libluna/Input/NdsGamepad.hpp>
#include <libluna/Input/NdsGamepadState.hpp>

namespace Luna::Input {
  class NdsGamepadDevice {
    public:
    NdsGamepadDevice(const NdsGamepadState* state);

    bool isDown(NdsGamepad::Button button) const;

    private:
    const NdsGamepadState* mState;
  };
}
