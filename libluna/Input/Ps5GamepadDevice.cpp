#include <libluna/Input/Ps5GamepadDevice.hpp>
#include <libluna/Input/Ps5Gamepad.hpp>
#include <libluna/Input/Ps5GamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

Ps5GamepadDevice::Ps5GamepadDevice(const Ps5GamepadState* state)
  : mState(state) {}

bool Ps5GamepadDevice::isDown(Ps5Gamepad::Button button) const {
  return mState && mState->isDown(button);
}

float Ps5GamepadDevice::getAxis(Ps5Gamepad::Axis axis) const {
  return mState ? mState->getAxis(axis) : 0.0f;
}

Vector2f Ps5GamepadDevice::getStick(Ps5Gamepad::Stick stick) const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  switch (stick) {
    case Ps5Gamepad::Stick::kLeftStick:
      return Vector2f(mState->getAxis(Ps5Gamepad::Axis::kLeftStickX),
                      mState->getAxis(Ps5Gamepad::Axis::kLeftStickY));
    case Ps5Gamepad::Stick::kRightStick:
      return Vector2f(mState->getAxis(Ps5Gamepad::Axis::kRightStickX),
                      mState->getAxis(Ps5Gamepad::Axis::kRightStickY));
    default:
      return Vector2f(0.0f, 0.0f);
  }
}
