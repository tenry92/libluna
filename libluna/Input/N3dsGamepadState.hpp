#pragma once

#include <array>

#include <libluna/Input/N3dsGamepad.hpp>

namespace Luna::Input {
  struct N3dsGamepadState {
    std::array<bool, static_cast<std::size_t>(N3dsGamepad::Button::kPower) + 1> buttonStates;
    std::array<float, static_cast<std::size_t>(N3dsGamepad::Axis::kStickY) + 1> axisStates;

    inline bool isDown(N3dsGamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(N3dsGamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(N3dsGamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(N3dsGamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
