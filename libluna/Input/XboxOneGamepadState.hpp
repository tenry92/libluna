#pragma once

#include <array>

#include <libluna/Input/XboxOneGamepad.hpp>

namespace Luna::Input {
  struct XboxOneGamepadState {
    std::array<bool, static_cast<std::size_t>(XboxOneGamepad::Button::kDpadRight) + 1> buttonStates;
    std::array<float, static_cast<std::size_t>(XboxOneGamepad::Axis::kRightTriggerAxis) + 1> axisStates;

    inline bool isDown(XboxOneGamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(XboxOneGamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(XboxOneGamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(XboxOneGamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
