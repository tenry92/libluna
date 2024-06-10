#pragma once

#include <memory>
#include <queue>

#include <libluna/ButtonEvent.hpp>
#include <libluna/String.hpp>

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
