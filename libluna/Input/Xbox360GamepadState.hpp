#pragma once

#include <array>

#include <libluna/Input/Xbox360Gamepad.hpp>

namespace Luna::Input {
  struct Xbox360GamepadState {
    std::array<bool, static_cast<std::size_t>(Xbox360Gamepad::Button::kDpadRight) + 1> buttonStates{};
    std::array<float, static_cast<std::size_t>(Xbox360Gamepad::Axis::kRightTriggerAxis) + 1> axisStates{};

    inline bool isDown(Xbox360Gamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(Xbox360Gamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(Xbox360Gamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(Xbox360Gamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
