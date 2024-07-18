#pragma once

#include <memory>
#include <queue>

#include <libluna/ButtonEvent.hpp>
#include <libluna/String.hpp>

/**
 * @page input Input
 *
 * Keyboard binding can either refer to a key's physical location (supported for
 * all keys) or by their virtual representation (supported only for keys that
 * produce output and may differ depending on the keyboard locale).
 *
 * The physical locations uses names in the format `Keyboard/Scancode/<Name>`.
 * The virtual keys uses names in the format `Keyboard/Keycode/<Character>`.
 *
 * The following keyboard keys are defined:
 *
 * | Physical | Virtual |
 * |----------|---------|
 * | `Keyboard/Scancode/A` - `Keyboard/Scancode/Z` | `Keyboard/Keycode/a` - `Keyboard/Keycode/z` |
 * | `Keyboard/Scancode/Grave` | *N/A* |
 * | `Keyboard/Scancode/0` - `Keyboard/Scancode/9` | `Keyboard/Keycode/0` - `Keyboard/Keycode/9` |
 * | `Keyboard/Scancode/Minus` | `Keyboard/Keycode/-` |
 * | `Keyboard/Scancode/Equals` | `Keyboard/Keycode/=` |
 * | `Keyboard/Scancode/Backspace` | *N/A*
 * | `Keyboard/Scancode/Escape` | *N/A*
 * | `Keyboard/Scancode/F1` - `Keyboard/Scancode/F12` | *N/A*
 * | `Keyboard/Scancode/Space` | *N/A* |
 * | `Keyboard/Scancode/Return` | *N/A* |
 * | `Keyboard/Scancode/Tab` | *N/A* |
 * | `Keyboard/Scancode/LeftShift`, `Keyboard/Scancode/RightShift` | *N/A* |
 * | `Keyboard/Scancode/LeftControl`, `Keyboard/Scancode/RightControl` | *N/A* |
 * | `Keyboard/Scancode/LeftSystem`, `Keyboard/Scancode/RightSystem` | *N/A* |
 * | `Keyboard/Scancode/LeftAlt`, `Keyboard/Scancode/RightAlt` | *N/A* |
 * | `Keyboard/Scancode/Pause` | *N/A* |
 * | `Keyboard/Scancode/ArrowUp`, `Keyboard/Scancode/ArrowDown`, `Keyboard/Scancode/ArrowLeft`, `Keyboard/Scancode/ArrowRight` | *N/A* |
 *
 * For controllers, the following primary buttons are defined:
 *
 * | Name | Xbox | Switch | PlayStation |
 * |------|------|--------|-------------|
 * | `Gamepad/Generic/ButtonSouth` | A | B | Cross
 * | `Gamepad/Generic/ButtonEast` | B | A | Circle
 * | `Gamepad/Generic/ButtonWest` | X | Y | Square
 * | `Gamepad/Generic/ButtonNorth` | Y | X | Triangle
 *
 * In addition, the following additional buttons and axes are defined:
 *
 * - `Gamepad/Generic/Back`, `Gamepad/Generic/Back`
 * - `Gamepad/Generic/Start`
 * - `Gamepad/Generic/LeftThumb` (when left stick is pressed)
 * - `Gamepad/Generic/RightThumb` (when right stick is pressed)
 * - `Gamepad/Generic/DPadUp`
 * - `Gamepad/Generic/DPadDown`
 * - `Gamepad/Generic/DPadLeft`
 * - `Gamepad/Generic/DPadRight`
 * - `Gamepad/Generic/LeftButton` (L, L1, LB)
 * - `Gamepad/Generic/RightButton` (R, R1, RB)
 * - `Gamepad/Generic/LeftTrigger` (LZ, L2, LT)
 * - `Gamepad/Generic/RightTrigger` (RZ, R2, RT)
 * - `Gamepad/Generic/LeftStickX`
 * - `Gamepad/Generic/LeftStickY`
 * - `Gamepad/Generic/RightStickX`
 * - `Gamepad/Generic/RightStickY`
 */

namespace Luna {
  class InputManager {
    public:
    InputManager();
    ~InputManager();
    void update(std::queue<ButtonEvent> *queue, float deltaTime);

    void addButtonBinding(const String &actionName, const String &buttonName);

    /**
     * @brief Check if the button was just pressed (not longer than \p buffer
     * seconds ago).
     */
    bool isButtonPressed(const String &actionName, float buffer = 0.0f);

    /**
     * @brief Check if the button is held since \p duration seconds.
     */
    bool isButtonHeld(const String &actionName, float duration = 0.0f);

    void setReturnUnused(bool returnUnused = true);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
