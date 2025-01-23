#include <libluna/InputManager.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

using namespace Luna;

InputManager::InputManager() = default;

InputManager::~InputManager() = default;

void InputManager::update(std::queue<ButtonEvent> *queue, float deltaTime) {
  std::for_each(
      mPressedActions.begin(), mPressedActions.end(),
      [deltaTime](auto &pair) { pair.second += deltaTime; }
  );

  std::queue<ButtonEvent> unusedQueue;

  while (!queue->empty()) {
    auto event = queue->front();
    queue->pop();

    auto buttonName = event.getName().c_str();

    if (mButtonBindings.count(buttonName)) {
      auto actionName = mButtonBindings.at(buttonName);

      if (event.isDown()) {
        mPressedActions.emplace(actionName, 0.0f);
      } else {
        mPressedActions.erase(actionName);
      }
    } else if (mReturnUnusedEvents) {
      unusedQueue.push(event);
    }
  }

  if (mReturnUnusedEvents) {
    *queue = std::move(unusedQueue);
  }
}

void InputManager::addButtonBinding(
    const String &actionName, const String &buttonName
) {
  mButtonBindings.emplace(buttonName.c_str(), actionName.c_str());
}

bool InputManager::isButtonPressed(const String &actionName, float buffer) {
  if (!mPressedActions.count(actionName.c_str())) {
    return false;
  }

  float duration = mPressedActions.at(actionName.c_str());

  return duration <= buffer;
}

bool InputManager::isButtonHeld(const String &actionName, float duration) {
  if (!mPressedActions.count(actionName.c_str())) {
    return false;
  }

  float presedDuration = mPressedActions.at(actionName.c_str());

  return presedDuration >= duration;
}

void InputManager::setReturnUnused(bool returnUnused) {
  mReturnUnusedEvents = returnUnused;
}
