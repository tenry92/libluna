#pragma once

#include <libluna/Input/Keyboard.hpp>
#include <libluna/Input/KeyboardState.hpp>

namespace Luna::Input {
  class KeyboardDevice {
    public:
    KeyboardDevice(const KeyboardState* state);

    bool isDown(Keyboard::Scancode scancode) const;
    bool isDown(char keycode) const;

    private:
    const KeyboardState* mState;
  };
}
