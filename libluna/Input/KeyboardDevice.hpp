#pragma once

#include <libluna/Input/Keyboard.hpp>
#include <libluna/Input/KeyboardState.hpp>

namespace Luna::Input {
  /**
   * @brief Represents a keyboard input device.
   *
   * @ingroup input input_devices
   */
  class KeyboardDevice {
    public:
    KeyboardDevice(const KeyboardState* state);

    /**
     * @brief Checks if a key (by physical position) is currently pressed down.
     */
    bool isDown(Keyboard::Scancode scancode) const;

    /**
     * @brief Checks if a key (locale-dependent) is currently pressed down.
     */
    bool isDown(char keycode) const;

    private:
    const KeyboardState* mState;
  };
}
