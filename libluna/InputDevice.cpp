#include <stdexcept>

#include <libluna/InputDevice.hpp>

using namespace Luna;
using namespace Luna::Input;

InputDevice::InputDevice(const InputDeviceVariant& device, int index) : mDevice(device), mIndex(index) {}

int InputDevice::getIndex() const {
  return mIndex;
}

#ifdef N64
N64GamepadDevice InputDevice::getN64Gamepad() const {
  return std::get<N64GamepadDevice>(mDevice);
}
#elif defined(__3DS__)
N3dsGamepadDevice InputDevice::getN3dsGamepad() const {
  return std::get<N3dsGamepadDevice>(mDevice);
}
#elif defined(__NDS__)
NdsGamepadDevice InputDevice::getNdsGamepad() const {
  return std::get<NdsGamepadDevice>(mDevice);
}
#else
bool InputDevice::isKeyboard() const {
  return std::holds_alternative<KeyboardDevice>(mDevice);
}

KeyboardDevice InputDevice::getKeyboard() const {
  return std::get<KeyboardDevice>(mDevice);
}

bool InputDevice::isPs5Gamepad() const {
  return std::holds_alternative<Ps5GamepadDevice>(mDevice);
}

Ps5GamepadDevice InputDevice::getPs5Gamepad() const {
  return std::get<Ps5GamepadDevice>(mDevice);
}

bool InputDevice::isXbox360Gamepad() const {
  return std::holds_alternative<Xbox360GamepadDevice>(mDevice);
}

Xbox360GamepadDevice InputDevice::getXbox360Gamepad() const {
  return std::get<Xbox360GamepadDevice>(mDevice);
}

bool InputDevice::isSwitchProGamepad() const {
  return std::holds_alternative<SwitchProGamepadDevice>(mDevice);
}

SwitchProGamepadDevice InputDevice::getSwitchProGamepad() const {
  return std::get<SwitchProGamepadDevice>(mDevice);
}

bool InputDevice::isXboxOneGamepad() const {
  return std::holds_alternative<XboxOneGamepadDevice>(mDevice);
}

XboxOneGamepadDevice InputDevice::getXboxOneGamepad() const {
  return std::get<XboxOneGamepadDevice>(mDevice);
}

bool InputDevice::isGamepad() const {
  return std::holds_alternative<Ps5GamepadDevice>(mDevice) ||
          std::holds_alternative<Xbox360GamepadDevice>(mDevice) ||
          std::holds_alternative<SwitchProGamepadDevice>(mDevice) ||
          std::holds_alternative<XboxOneGamepadDevice>(mDevice);
}

GenericGamepadDevice InputDevice::getGenericGamepad() const {
  if (std::holds_alternative<Ps5GamepadDevice>(mDevice)) {
    return GenericGamepadDevice(std::get<Ps5GamepadDevice>(mDevice));
  } else if (std::holds_alternative<Xbox360GamepadDevice>(mDevice)) {
    return GenericGamepadDevice(std::get<Xbox360GamepadDevice>(mDevice));
  } else if (std::holds_alternative<SwitchProGamepadDevice>(mDevice)) {
    return GenericGamepadDevice(std::get<SwitchProGamepadDevice>(mDevice));
  } else if (std::holds_alternative<XboxOneGamepadDevice>(mDevice)) {
    return GenericGamepadDevice(std::get<XboxOneGamepadDevice>(mDevice));
  }
  
  // This should not happen if isGamepad() was checked first
  throw std::runtime_error("Current device is not a gamepad");
}
#endif
