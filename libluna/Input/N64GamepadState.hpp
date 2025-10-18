#pragma once

#include <array>

#include <libluna/Input/N64Gamepad.hpp>

namespace Luna::Input {
  struct N64GamepadState {
    std::array<bool, static_cast<std::size_t>(N64Gamepad::Button::kCRight) + 1> buttonStates{};
    std::array<float, static_cast<std::size_t>(N64Gamepad::Axis::kStickY) + 1> axisStates{};

    inline bool isDown(N64Gamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(N64Gamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(N64Gamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(N64Gamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
