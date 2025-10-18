#include <libluna/Input/GenericGamepadDevice.hpp>

using namespace Luna;
using namespace Luna::Input;

GenericGamepadDevice::GenericGamepadDevice(const GamepadDeviceVariant& device)
  : mDevice(device) {}

bool GenericGamepadDevice::isDown(GenericGamepad::Button button) const {
  return std::visit([&button](const auto& dev) {
    using DevType = std::decay_t<decltype(dev)>;
    if constexpr (std::is_same_v<DevType, Ps5GamepadDevice>) {
      switch (button) {
        case GenericGamepad::Button::kSouth:
          return dev.isDown(Ps5Gamepad::Button::kCross);
        case GenericGamepad::Button::kEast:
          return dev.isDown(Ps5Gamepad::Button::kCircle);
        case GenericGamepad::Button::kWest:
          return dev.isDown(Ps5Gamepad::Button::kSquare);
        case GenericGamepad::Button::kNorth:
          return dev.isDown(Ps5Gamepad::Button::kTriangle);
        case GenericGamepad::Button::kBack:
          return dev.isDown(Ps5Gamepad::Button::kCreate);
        case GenericGamepad::Button::kStart:
          return dev.isDown(Ps5Gamepad::Button::kOptions);
        case GenericGamepad::Button::kGuide:
          return dev.isDown(Ps5Gamepad::Button::kPS);
        case GenericGamepad::Button::kLeftBumper:
          return dev.isDown(Ps5Gamepad::Button::kL1);
        case GenericGamepad::Button::kRightBumper:
          return dev.isDown(Ps5Gamepad::Button::kR1);
        case GenericGamepad::Button::kLeftTrigger:
          return dev.isDown(Ps5Gamepad::Button::kL2);
        case GenericGamepad::Button::kRightTrigger:
          return dev.isDown(Ps5Gamepad::Button::kR2);
        case GenericGamepad::Button::kLeftStickButton:
          return dev.isDown(Ps5Gamepad::Button::kLeftStickButton);
        case GenericGamepad::Button::kRightStickButton:
          return dev.isDown(Ps5Gamepad::Button::kRightStickButton);
        case GenericGamepad::Button::kDpadUp:
          return dev.isDown(Ps5Gamepad::Button::kDpadUp);
        case GenericGamepad::Button::kDpadDown:
          return dev.isDown(Ps5Gamepad::Button::kDpadDown);
        case GenericGamepad::Button::kDpadLeft:
          return dev.isDown(Ps5Gamepad::Button::kDpadLeft);
        case GenericGamepad::Button::kDpadRight:
          return dev.isDown(Ps5Gamepad::Button::kDpadRight);
        default:
          return false;
      }
    } else if constexpr (std::is_same_v<DevType, Xbox360GamepadDevice>) {
      switch (button) {
        case GenericGamepad::Button::kSouth:
          return dev.isDown(Xbox360Gamepad::Button::kA);
        case GenericGamepad::Button::kEast:
          return dev.isDown(Xbox360Gamepad::Button::kB);
        case GenericGamepad::Button::kWest:
          return dev.isDown(Xbox360Gamepad::Button::kX);
        case GenericGamepad::Button::kNorth:
          return dev.isDown(Xbox360Gamepad::Button::kY);
        case GenericGamepad::Button::kBack:
          return dev.isDown(Xbox360Gamepad::Button::kBack);
        case GenericGamepad::Button::kStart:
          return dev.isDown(Xbox360Gamepad::Button::kStart);
        case GenericGamepad::Button::kGuide:
          return dev.isDown(Xbox360Gamepad::Button::kGuide);
        case GenericGamepad::Button::kLeftBumper:
          return dev.isDown(Xbox360Gamepad::Button::kLeftBumper);
        case GenericGamepad::Button::kRightBumper:
          return dev.isDown(Xbox360Gamepad::Button::kRightBumper);
        case GenericGamepad::Button::kLeftTrigger:
          return dev.isDown(Xbox360Gamepad::Button::kLeftTrigger);
        case GenericGamepad::Button::kRightTrigger:
          return dev.isDown(Xbox360Gamepad::Button::kRightTrigger);
        case GenericGamepad::Button::kLeftStickButton:
          return dev.isDown(Xbox360Gamepad::Button::kLeftStickButton);
        case GenericGamepad::Button::kRightStickButton:
          return dev.isDown(Xbox360Gamepad::Button::kRightStickButton);
        case GenericGamepad::Button::kDpadUp:
          return dev.isDown(Xbox360Gamepad::Button::kDpadUp);
        case GenericGamepad::Button::kDpadDown:
          return dev.isDown(Xbox360Gamepad::Button::kDpadDown);
        case GenericGamepad::Button::kDpadLeft:
          return dev.isDown(Xbox360Gamepad::Button::kDpadLeft);
        case GenericGamepad::Button::kDpadRight:
          return dev.isDown(Xbox360Gamepad::Button::kDpadRight);
        default:
          return false;
      }
    } else if constexpr (std::is_same_v<DevType, SwitchProGamepadDevice>) {
      switch (button) {
        case GenericGamepad::Button::kSouth:
          return dev.isDown(SwitchProGamepad::Button::kB);
        case GenericGamepad::Button::kEast:
          return dev.isDown(SwitchProGamepad::Button::kA);
        case GenericGamepad::Button::kWest:
          return dev.isDown(SwitchProGamepad::Button::kY);
        case GenericGamepad::Button::kNorth:
          return dev.isDown(SwitchProGamepad::Button::kX);
        case GenericGamepad::Button::kBack:
          return dev.isDown(SwitchProGamepad::Button::kMinus);
        case GenericGamepad::Button::kStart:
          return dev.isDown(SwitchProGamepad::Button::kPlus);
        case GenericGamepad::Button::kGuide:
          return dev.isDown(SwitchProGamepad::Button::kHome);
        case GenericGamepad::Button::kLeftBumper:
          return dev.isDown(SwitchProGamepad::Button::kL);
        case GenericGamepad::Button::kRightBumper:
          return dev.isDown(SwitchProGamepad::Button::kR);
        case GenericGamepad::Button::kLeftTrigger:
          return dev.isDown(SwitchProGamepad::Button::kZL);
        case GenericGamepad::Button::kRightTrigger:
          return dev.isDown(SwitchProGamepad::Button::kZR);
        case GenericGamepad::Button::kLeftStickButton:
          return dev.isDown(SwitchProGamepad::Button::kLeftStickButton);
        case GenericGamepad::Button::kRightStickButton:
          return dev.isDown(SwitchProGamepad::Button::kRightStickButton);
        case GenericGamepad::Button::kDpadUp:
          return dev.isDown(SwitchProGamepad::Button::kDpadUp);
        case GenericGamepad::Button::kDpadDown:
          return dev.isDown(SwitchProGamepad::Button::kDpadDown);
        case GenericGamepad::Button::kDpadLeft:
          return dev.isDown(SwitchProGamepad::Button::kDpadLeft);
        case GenericGamepad::Button::kDpadRight:
          return dev.isDown(SwitchProGamepad::Button::kDpadRight);
        default:
          return false;
      }
    } else if constexpr (std::is_same_v<DevType, XboxOneGamepadDevice>) {
      switch (button) {
        case GenericGamepad::Button::kSouth:
          return dev.isDown(XboxOneGamepad::Button::kA);
        case GenericGamepad::Button::kEast:
          return dev.isDown(XboxOneGamepad::Button::kB);
        case GenericGamepad::Button::kWest:
          return dev.isDown(XboxOneGamepad::Button::kX);
        case GenericGamepad::Button::kNorth:
          return dev.isDown(XboxOneGamepad::Button::kY);
        case GenericGamepad::Button::kBack:
          return dev.isDown(XboxOneGamepad::Button::kView);
        case GenericGamepad::Button::kStart:
          return dev.isDown(XboxOneGamepad::Button::kMenu);
        case GenericGamepad::Button::kGuide:
          return dev.isDown(XboxOneGamepad::Button::kXbox);
        case GenericGamepad::Button::kLeftBumper:
          return dev.isDown(XboxOneGamepad::Button::kLeftBumper);
        case GenericGamepad::Button::kRightBumper:
          return dev.isDown(XboxOneGamepad::Button::kRightBumper);
        case GenericGamepad::Button::kLeftTrigger:
          return dev.isDown(XboxOneGamepad::Button::kLeftTrigger);
        case GenericGamepad::Button::kRightTrigger:
          return dev.isDown(XboxOneGamepad::Button::kRightTrigger);
        case GenericGamepad::Button::kLeftStickButton:
          return dev.isDown(XboxOneGamepad::Button::kLeftStickButton);
        case GenericGamepad::Button::kRightStickButton:
          return dev.isDown(XboxOneGamepad::Button::kRightStickButton);
        case GenericGamepad::Button::kDpadUp:
          return dev.isDown(XboxOneGamepad::Button::kDpadUp);
        case GenericGamepad::Button::kDpadDown:
          return dev.isDown(XboxOneGamepad::Button::kDpadDown);
        case GenericGamepad::Button::kDpadLeft:
          return dev.isDown(XboxOneGamepad::Button::kDpadLeft);
        case GenericGamepad::Button::kDpadRight:
          return dev.isDown(XboxOneGamepad::Button::kDpadRight);
        default:
          return false;
      }
    } else {
      return false;
    }
  }, mDevice);
}

float GenericGamepadDevice::getAxis(GenericGamepad::Axis axis) const {
  return std::visit([&axis](const auto& dev) -> float {
    using DevType = std::decay_t<decltype(dev)>;
    if constexpr (std::is_same_v<DevType, Ps5GamepadDevice>) {
      switch (axis) {
        case GenericGamepad::Axis::kLeftStickX:
          return dev.getAxis(Ps5Gamepad::Axis::kLeftStickX);
        case GenericGamepad::Axis::kLeftStickY:
          return dev.getAxis(Ps5Gamepad::Axis::kLeftStickY);
        case GenericGamepad::Axis::kRightStickX:
          return dev.getAxis(Ps5Gamepad::Axis::kRightStickX);
        case GenericGamepad::Axis::kRightStickY:
          return dev.getAxis(Ps5Gamepad::Axis::kRightStickY);
        case GenericGamepad::Axis::kLeftTriggerAxis:
          return dev.getAxis(Ps5Gamepad::Axis::kL2Axis);
        case GenericGamepad::Axis::kRightTriggerAxis:
          return dev.getAxis(Ps5Gamepad::Axis::kR2Axis);
        default:
          return 0.0f;
      }
    } else if constexpr (std::is_same_v<DevType, Xbox360GamepadDevice>) {
      switch (axis) {
        case GenericGamepad::Axis::kLeftStickX:
          return dev.getAxis(Xbox360Gamepad::Axis::kLeftStickX);
        case GenericGamepad::Axis::kLeftStickY:
          return dev.getAxis(Xbox360Gamepad::Axis::kLeftStickY);
        case GenericGamepad::Axis::kRightStickX:
          return dev.getAxis(Xbox360Gamepad::Axis::kRightStickX);
        case GenericGamepad::Axis::kRightStickY:
          return dev.getAxis(Xbox360Gamepad::Axis::kRightStickY);
        case GenericGamepad::Axis::kLeftTriggerAxis:
          return dev.getAxis(Xbox360Gamepad::Axis::kLeftTriggerAxis);
        case GenericGamepad::Axis::kRightTriggerAxis:
          return dev.getAxis(Xbox360Gamepad::Axis::kRightTriggerAxis);
        default:
          return 0.0f;
      }
    } else if constexpr (std::is_same_v<DevType, SwitchProGamepadDevice>) {
      switch (axis) {
        case GenericGamepad::Axis::kLeftStickX:
          return dev.getAxis(SwitchProGamepad::Axis::kLeftStickX);
        case GenericGamepad::Axis::kLeftStickY:
          return dev.getAxis(SwitchProGamepad::Axis::kLeftStickY);
        case GenericGamepad::Axis::kRightStickX:
          return dev.getAxis(SwitchProGamepad::Axis::kRightStickX);
        case GenericGamepad::Axis::kRightStickY:
          return dev.getAxis(SwitchProGamepad::Axis::kRightStickY);
        case GenericGamepad::Axis::kLeftTriggerAxis:
            return dev.isDown(SwitchProGamepad::Button::kZL) ? 1.0f : 0.0f;
        case GenericGamepad::Axis::kRightTriggerAxis:
          return dev.isDown(SwitchProGamepad::Button::kZR) ? 1.0f : 0.0f;
        default:
          return 0.0f;
      }
    } else if constexpr (std::is_same_v<DevType, XboxOneGamepadDevice>) {
      switch (axis) {
        case GenericGamepad::Axis::kLeftStickX:
          return dev.getAxis(XboxOneGamepad::Axis::kLeftStickX);
        case GenericGamepad::Axis::kLeftStickY:
          return dev.getAxis(XboxOneGamepad::Axis::kLeftStickY);
        case GenericGamepad::Axis::kRightStickX:
          return dev.getAxis(XboxOneGamepad::Axis::kRightStickX);
        case GenericGamepad::Axis::kRightStickY:
          return dev.getAxis(XboxOneGamepad::Axis::kRightStickY);
        case GenericGamepad::Axis::kLeftTriggerAxis:
          return dev.getAxis(XboxOneGamepad::Axis::kLeftTriggerAxis);
        case GenericGamepad::Axis::kRightTriggerAxis:
          return dev.getAxis(XboxOneGamepad::Axis::kRightTriggerAxis);
        default:
          return 0.0f;
      }
    } else {
      return 0.0f;
    }
  }, mDevice);
}

Vector2f GenericGamepadDevice::getStick(GenericGamepad::Stick stick) const {
  switch (stick) {
    case GenericGamepad::Stick::kLeftStick:
      return Vector2f(getAxis(GenericGamepad::Axis::kLeftStickX),
                      getAxis(GenericGamepad::Axis::kLeftStickY));
    case GenericGamepad::Stick::kRightStick:
      return Vector2f(getAxis(GenericGamepad::Axis::kRightStickX),
                      getAxis(GenericGamepad::Axis::kRightStickY));
    default:
      return Vector2f(0.0f, 0.0f);
  }
}
