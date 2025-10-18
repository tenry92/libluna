#include <libluna/Input/N3dsGamepadDevice.hpp>
#include <libluna/Input/N3dsGamepad.hpp>
#include <libluna/Input/N3dsGamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

N3dsGamepadDevice::N3dsGamepadDevice(const N3dsGamepadState* state)
  : mState(state) {}

bool N3dsGamepadDevice::isDown(N3dsGamepad::Button button) const {
  return mState && mState->isDown(button);
}

float N3dsGamepadDevice::getAxis(N3dsGamepad::Axis axis) const {
  if (!mState) {
    return 0.0f;
  }

  switch (axis) {
    case N3dsGamepad::Axis::kStickX:
      return 0.0f; // Placeholder, replace with actual stick X value retrieval
    case N3dsGamepad::Axis::kStickY:
      return 0.0f; // Placeholder, replace with actual stick Y value retrieval
    default:
      return 0.0f;
  }
}

Vector2f N3dsGamepadDevice::getStick() const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  return Vector2f(mState->getAxis(N3dsGamepad::Axis::kStickX),
                  mState->getAxis(N3dsGamepad::Axis::kStickY));
}
