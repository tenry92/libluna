#include <libluna/InputManager.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

using namespace Luna;

class InputManager::impl {
  public:
  std::map<std::string, float> mPressedActions;
  std::map<std::string, std::string> mButtonBindings;
  bool mReturnUnusedEvents{false};
};

InputManager::InputManager() : mImpl{std::make_unique<impl>()} {}

InputManager::~InputManager() = default;

void InputManager::update(std::queue<ButtonEvent> *queue, float deltaTime) {
  std::for_each(
      mImpl->mPressedActions.begin(), mImpl->mPressedActions.end(),
      [deltaTime](auto &pair) { pair.second += deltaTime; }
  );

  std::queue<ButtonEvent> unusedQueue;

  while (!queue->empty()) {
    auto event = queue->front();
    queue->pop();

    auto buttonName = event.getName().s_str();

    if (mImpl->mButtonBindings.count(buttonName)) {
      auto actionName = mImpl->mButtonBindings.at(buttonName);

      if (event.isDown()) {
        mImpl->mPressedActions.emplace(actionName, 0.0f);
      } else {
        mImpl->mPressedActions.erase(actionName);
      }
    } else if (mImpl->mReturnUnusedEvents) {
      unusedQueue.push(event);
    }
  }

  if (mImpl->mReturnUnusedEvents) {
    *queue = std::move(unusedQueue);
  }
}

void InputManager::addButtonBinding(
    const String &actionName, const String &buttonName
) {
  mImpl->mButtonBindings.emplace(buttonName.s_str(), actionName.s_str());
}

bool InputManager::isButtonPressed(const String &actionName, float buffer) {
  if (!mImpl->mPressedActions.count(actionName.s_str())) {
    return false;
  }

  float duration = mImpl->mPressedActions.at(actionName.s_str());

  return duration <= buffer;
}

bool InputManager::isButtonHeld(const String &actionName, float duration) {
  if (!mImpl->mPressedActions.count(actionName.s_str())) {
    return false;
  }

  float presedDuration = mImpl->mPressedActions.at(actionName.s_str());

  return presedDuration >= duration;
}

void InputManager::setReturnUnused(bool returnUnused) {
  mImpl->mReturnUnusedEvents = returnUnused;
}
