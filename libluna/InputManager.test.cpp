#include <libluna/InputManager.hpp>
#include <libluna/Test.hpp>

using namespace Luna;

int main(int, char**) {
  TEST("isButtonPressed 0s", []() {
    InputManager inputManager;
    inputManager.addButtonBinding("jump", "A");

    ASSERT(!inputManager.isButtonPressed("jump"), "!jump");

    inputManager.handleButtonEvent(ButtonEvent("A", true));
    ASSERT(inputManager.isButtonPressed("jump"), "jump");

    inputManager.update(0.1f);
    ASSERT(!inputManager.isButtonPressed("jump"), "no jump");

    inputManager.handleButtonEvent(ButtonEvent("A", false));
    inputManager.update(0.1f);
    ASSERT(!inputManager.isButtonPressed("jump"), "no longer jump");
  });

  TEST("isButtonPressed 0.5s", []() {
    InputManager inputManager;
    inputManager.addButtonBinding("jump", "A");
    float buffer = 0.5f;

    ASSERT(!inputManager.isButtonPressed("jump", buffer), "!jump");

    inputManager.handleButtonEvent(ButtonEvent("A", true));
    inputManager.update(0.1f);
    ASSERT(inputManager.isButtonPressed("jump", buffer), "jump");

    inputManager.update(0.1f);
    ASSERT(inputManager.isButtonPressed("jump", buffer), "still jump");

    inputManager.update(buffer);
    ASSERT(!inputManager.isButtonPressed("jump", buffer), "no longer jump");
  });

  TEST("isButtonHeld 0.5s", []() {
    InputManager inputManager;
    inputManager.addButtonBinding("jump", "A");
    float duration = 0.5f;

    ASSERT(!inputManager.isButtonHeld("jump", duration), "!jump");

    inputManager.handleButtonEvent(ButtonEvent("A", true));
    inputManager.update(0.1f);
    ASSERT(!inputManager.isButtonHeld("jump", duration), "not jump");

    inputManager.update(0.1f);
    ASSERT(!inputManager.isButtonHeld("jump", duration), "still not jump");

    inputManager.update(duration);
    ASSERT(inputManager.isButtonHeld("jump", duration), "jump");
  });

  return runTests();
}
