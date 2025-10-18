#pragma once

#include <array>

#include <libluna/Input/SwitchProGamepad.hpp>

namespace Luna::Input {
  struct SwitchProGamepadState {
    std::array<bool, static_cast<std::size_t>(SwitchProGamepad::Button::kCapture) + 1> buttonStates{};
    std::array<float, static_cast<std::size_t>(SwitchProGamepad::Axis::kRightStickY) + 1> axisStates{};

    inline bool isDown(SwitchProGamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(SwitchProGamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(SwitchProGamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(SwitchProGamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
