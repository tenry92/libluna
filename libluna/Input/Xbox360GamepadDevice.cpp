#include <libluna/Input/Xbox360GamepadDevice.hpp>
#include <libluna/Input/Xbox360Gamepad.hpp>
#include <libluna/Input/Xbox360GamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

Xbox360GamepadDevice::Xbox360GamepadDevice(const Xbox360GamepadState* state)
  : mState(state) {}

bool Xbox360GamepadDevice::isDown(Xbox360Gamepad::Button button) const {
  return mState && mState->isDown(button);
}

float Xbox360GamepadDevice::getAxis(Xbox360Gamepad::Axis axis) const {
  return mState ? mState->getAxis(axis) : 0.0f;
}

Vector2f Xbox360GamepadDevice::getStick(Xbox360Gamepad::Stick stick) const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  switch (stick) {
    case Xbox360Gamepad::Stick::kLeftStick:
      return Vector2f(mState->getAxis(Xbox360Gamepad::Axis::kLeftStickX),
                      mState->getAxis(Xbox360Gamepad::Axis::kLeftStickY));
    case Xbox360Gamepad::Stick::kRightStick:
      return Vector2f(mState->getAxis(Xbox360Gamepad::Axis::kRightStickX),
                      mState->getAxis(Xbox360Gamepad::Axis::kRightStickY));
    default:
      return Vector2f(0.0f, 0.0f);
  }
}
