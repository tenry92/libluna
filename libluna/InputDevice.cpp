#include <stdexcept>

#include <libluna/InputDevice.hpp>

using namespace Luna;

InputDevice::InputDevice(const InputDeviceVariant& device, int index) : mDevice(device), mIndex(index) {}

int InputDevice::getIndex() const {
  return mIndex;
}

#ifdef N64
N64GamepadDevice InputDevice::getN64Gamepad() {
  return std::get<N64GamepadDevice>(mDevice);
}
#elif defined(__3DS__)
N3dsGamepadDevice InputDevice::getN3dsGamepad() {
  return std::get<N3dsGamepadDevice>(mDevice);
}
#elif defined(NDS)
NdsGamepadDevice InputDevice::getNdsGamepad() {
  return std::get<NdsGamepadDevice>(mDevice);
}
#else
bool InputDevice::isKeyboard() const {
  return std::holds_alternative<KeyboardDevice>(mDevice);
}

KeyboardDevice InputDevice::getKeyboard() {
  return std::get<KeyboardDevice>(mDevice);
}

bool InputDevice::isPs5Gamepad() const {
  return std::holds_alternative<Ps5GamepadDevice>(mDevice);
}

Ps5GamepadDevice InputDevice::getPs5Gamepad() {
  return std::get<Ps5GamepadDevice>(mDevice);
}

bool InputDevice::isXbox360Gamepad() const {
  return std::holds_alternative<Xbox360GamepadDevice>(mDevice);
}

Xbox360GamepadDevice InputDevice::getXbox360Gamepad() {
  return std::get<Xbox360GamepadDevice>(mDevice);
}

bool InputDevice::isSwitchProGamepad() const {
  return std::holds_alternative<SwitchProGamepadDevice>(mDevice);
}

SwitchProGamepadDevice InputDevice::getSwitchProGamepad() {
  return std::get<SwitchProGamepadDevice>(mDevice);
}

bool InputDevice::isXboxOneGamepad() const {
  return std::holds_alternative<XboxOneGamepadDevice>(mDevice);
}

XboxOneGamepadDevice InputDevice::getXboxOneGamepad() {
  return std::get<XboxOneGamepadDevice>(mDevice);
}

bool InputDevice::isGamepad() {
  return std::holds_alternative<Ps5GamepadDevice>(mDevice) ||
          std::holds_alternative<Xbox360GamepadDevice>(mDevice) ||
          std::holds_alternative<SwitchProGamepadDevice>(mDevice) ||
          std::holds_alternative<XboxOneGamepadDevice>(mDevice);
}

GenericGamepadDevice InputDevice::getGenericGamepad() {
  if (std::holds_alternative<Ps5GamepadDevice>(mDevice)) {
    static GenericGamepadDevice genericDevice(std::get<Ps5GamepadDevice>(mDevice));
    return genericDevice;
  } else if (std::holds_alternative<Xbox360GamepadDevice>(mDevice)) {
    static GenericGamepadDevice genericDevice(std::get<Xbox360GamepadDevice>(mDevice));
    return genericDevice;
  } else if (std::holds_alternative<SwitchProGamepadDevice>(mDevice)) {
    static GenericGamepadDevice genericDevice(std::get<SwitchProGamepadDevice>(mDevice));
    return genericDevice;
  } else if (std::holds_alternative<XboxOneGamepadDevice>(mDevice)) {
    static GenericGamepadDevice genericDevice(std::get<XboxOneGamepadDevice>(mDevice));
    return genericDevice;
  }
  
  // This should not happen if isGamepad() was checked first
  throw std::runtime_error("Current device is not a gamepad");
}
#endif
