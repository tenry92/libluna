#pragma once

#include <array>

#include <libluna/Input/Keyboard.hpp>

namespace {
  constexpr const char kFirstKeycode = ' ';
  constexpr const char kLastKeycode = '~';
}

namespace Luna::Input {
  struct KeyboardState {
    std::array<bool, static_cast<std::size_t>(Keyboard::Scancode::kUnknown)> keyStates;
    std::array<bool, kLastKeycode - kFirstKeycode + 1> keycodeStates;

    inline bool isDown(Keyboard::Scancode scancode) const {
      return keyStates[static_cast<std::size_t>(scancode)];
    }

    inline bool isDown(char keycode) const {
      if (keycode < kFirstKeycode || keycode > kLastKeycode) {
        return false;
      }

      return keycodeStates[static_cast<std::size_t>(keycode - kFirstKeycode)];
    }

    inline void setDown(Keyboard::Scancode scancode, bool down) {
      keyStates[static_cast<std::size_t>(scancode)] = down;
    }

    inline void setDown(char keycode, bool down) {
      if (keycode < kFirstKeycode || keycode > kLastKeycode) {
        return;
      }

      keycodeStates[static_cast<std::size_t>(keycode - kFirstKeycode)] = down;
    }
  };
}
