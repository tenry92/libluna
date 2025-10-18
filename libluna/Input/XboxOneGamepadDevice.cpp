#include <libluna/Input/XboxOneGamepadDevice.hpp>
#include <libluna/Input/XboxOneGamepad.hpp>
#include <libluna/Input/XboxOneGamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

XboxOneGamepadDevice::XboxOneGamepadDevice(const XboxOneGamepadState* state)
  : mState(state) {}

bool XboxOneGamepadDevice::isDown(XboxOneGamepad::Button button) const {
  return mState && mState->isDown(button);
}

float XboxOneGamepadDevice::getAxis(XboxOneGamepad::Axis axis) const {
  return mState ? mState->getAxis(axis) : 0.0f;
}

Vector2f XboxOneGamepadDevice::getStick(XboxOneGamepad::Stick stick) const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  switch (stick) {
    case XboxOneGamepad::Stick::kLeftStick:
      return Vector2f(mState->getAxis(XboxOneGamepad::Axis::kLeftStickX),
                      mState->getAxis(XboxOneGamepad::Axis::kLeftStickY));
    case XboxOneGamepad::Stick::kRightStick:
      return Vector2f(mState->getAxis(XboxOneGamepad::Axis::kRightStickX),
                      mState->getAxis(XboxOneGamepad::Axis::kRightStickY));
    default:
      return Vector2f(0.0f, 0.0f);
  }
}
