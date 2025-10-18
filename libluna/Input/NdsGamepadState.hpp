#pragma once

#include <array>

#include <libluna/Input/NdsGamepad.hpp>

namespace Luna::Input {
  struct NdsGamepadState {
    std::array<bool, static_cast<std::size_t>(NdsGamepad::Button::kDpadRight) + 1> buttonStates;

    inline bool isDown(NdsGamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline void setDown(NdsGamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }
  };
}
