#include <libluna/Input/SwitchProGamepadDevice.hpp>
#include <libluna/Input/SwitchProGamepad.hpp>
#include <libluna/Input/SwitchProGamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

SwitchProGamepadDevice::SwitchProGamepadDevice(const SwitchProGamepadState* state)
  : mState(state) {}

bool SwitchProGamepadDevice::isDown(SwitchProGamepad::Button button) const {
  return mState && mState->isDown(button);
}

float SwitchProGamepadDevice::getAxis(SwitchProGamepad::Axis axis) const {
  return mState ? mState->getAxis(axis) : 0.0f;
}

Vector2f SwitchProGamepadDevice::getStick(SwitchProGamepad::Stick stick) const {
  if (!mState) {
    return Vector2f(0.0f, 0.0f);
  }

  switch (stick) {
    case SwitchProGamepad::Stick::kLeftStick:
      return Vector2f(mState->getAxis(SwitchProGamepad::Axis::kLeftStickX),
                      mState->getAxis(SwitchProGamepad::Axis::kLeftStickY));
    case SwitchProGamepad::Stick::kRightStick:
      return Vector2f(mState->getAxis(SwitchProGamepad::Axis::kRightStickX),
                      mState->getAxis(SwitchProGamepad::Axis::kRightStickY));
    default:
      return Vector2f(0.0f, 0.0f);
  }
}
