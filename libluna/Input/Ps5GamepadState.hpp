#pragma once

#include <array>

#include <libluna/Input/Ps5Gamepad.hpp>

namespace Luna::Input {
  /**
   * @brief Represents the state of a PlayStation 5 gamepad.
   *
   * @ingroup input
   */
  struct Ps5GamepadState {
    std::array<bool, static_cast<std::size_t>(Ps5Gamepad::Button::kMute) + 1> buttonStates{};
    std::array<float, static_cast<std::size_t>(Ps5Gamepad::Axis::kR2Axis) + 1> axisStates{};

    inline bool isDown(Ps5Gamepad::Button button) const {
      return buttonStates[static_cast<std::size_t>(button)];
    }

    inline float getAxis(Ps5Gamepad::Axis axis) const {
      return axisStates[static_cast<std::size_t>(axis)];
    }

    inline void setDown(Ps5Gamepad::Button button, bool down) {
      buttonStates[static_cast<std::size_t>(button)] = down;
    }

    inline void setAxis(Ps5Gamepad::Axis axis, float value) {
      axisStates[static_cast<std::size_t>(axis)] = value;
    }
  };
}
