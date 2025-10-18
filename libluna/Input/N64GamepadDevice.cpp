#include <libluna/Input/N64GamepadDevice.hpp>
#include <libluna/Input/N64Gamepad.hpp>
#include <libluna/Input/N64GamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

N64GamepadDevice::N64GamepadDevice(const N64GamepadState* state)
  : mState(state) {}

bool N64GamepadDevice::isDown(N64Gamepad::Button button) const {
  return mState && mState->isDown(button);
}

float N64GamepadDevice::getAxis(N64Gamepad::Axis axis) const {
  return mState ? mState->getAxis(axis) : 0.0f;
}

Vector2f N64GamepadDevice::getStick() const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  return Vector2f(mState->getAxis(N64Gamepad::Axis::kStickX),
                  mState->getAxis(N64Gamepad::Axis::kStickY));
}
