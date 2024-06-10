#include <libluna/InputManager.hpp>
#include <libluna/Test.hpp>

using namespace Luna;

int main(int, char **) {
  TEST("isButtonPressed 0s", []() {
    InputManager inputManager;
    std::queue<ButtonEvent> queue;
    inputManager.addButtonBinding("jump", "A");

    ASSERT(!inputManager.isButtonPressed("jump"), "!jump");

    queue.emplace(ButtonEvent("A", true));
    inputManager.update(&queue, 0.1f);
    ASSERT(inputManager.isButtonPressed("jump"), "jump");

    inputManager.update(&queue, 0.1f);
    ASSERT(!inputManager.isButtonPressed("jump"), "no jump");

    queue.emplace(ButtonEvent("A", false));
    inputManager.update(&queue, 0.1f);
    ASSERT(!inputManager.isButtonPressed("jump"), "no longer jump");
  });

  TEST("isButtonPressed 0.5s", []() {
    InputManager inputManager;
    std::queue<ButtonEvent> queue;
    inputManager.addButtonBinding("jump", "A");
    float buffer = 0.5f;

    ASSERT(!inputManager.isButtonPressed("jump", buffer), "!jump");

    queue.emplace(ButtonEvent("A", true));
    inputManager.update(&queue, 0.1f);
    ASSERT(inputManager.isButtonPressed("jump", buffer), "jump");

    inputManager.update(&queue, 0.1f);
    ASSERT(inputManager.isButtonPressed("jump", buffer), "still jump");

    inputManager.update(&queue, buffer);
    ASSERT(!inputManager.isButtonPressed("jump", buffer), "no longer jump");
  });

  TEST("isButtonHeld 0.5s", []() {
    InputManager inputManager;
    std::queue<ButtonEvent> queue;
    inputManager.addButtonBinding("jump", "A");
    float duration = 0.5f;

    ASSERT(!inputManager.isButtonHeld("jump", duration), "!jump");

    queue.emplace(ButtonEvent("A", true));
    inputManager.update(&queue, 0.1f);
    ASSERT(!inputManager.isButtonHeld("jump", duration), "not jump");

    inputManager.update(&queue, 0.1f);
    ASSERT(!inputManager.isButtonHeld("jump", duration), "still not jump");

    inputManager.update(&queue, duration);
    ASSERT(inputManager.isButtonHeld("jump", duration), "jump");
  });

  TEST("clear queue", []() {
    InputManager inputManager;
    std::queue<ButtonEvent> queue;
    inputManager.addButtonBinding("jump", "A");

    queue.emplace(ButtonEvent("A", true));
    queue.emplace(ButtonEvent("A", false));
    queue.emplace(ButtonEvent("A", true));
    queue.emplace(ButtonEvent("A", false));
    queue.emplace(ButtonEvent("B", true));
    queue.emplace(ButtonEvent("B", false));
    inputManager.update(&queue, 0.1f);
    ASSERT_EQL(0, queue.size(), "queue is empty");
  });

  TEST("return unused events", []() {
    InputManager inputManager;
    std::queue<ButtonEvent> queue;
    inputManager.setReturnUnused(true);
    inputManager.addButtonBinding("jump", "A");

    queue.emplace(ButtonEvent("A", true));
    queue.emplace(ButtonEvent("A", false));
    queue.emplace(ButtonEvent("A", true));
    queue.emplace(ButtonEvent("A", false));
    queue.emplace(ButtonEvent("B", true));
    queue.emplace(ButtonEvent("B", false));
    inputManager.update(&queue, 0.1f);
    ASSERT_EQL(2, queue.size(), "queue contains 2 events");
  });

  return runTests();
}
