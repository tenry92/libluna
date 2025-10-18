#pragma once

#include <variant>

#include <libluna/Input/GenericGamepad.hpp>
#include <libluna/Input/Ps5GamepadDevice.hpp>
#include <libluna/Input/SwitchProGamepadDevice.hpp>
#include <libluna/Input/Xbox360GamepadDevice.hpp>
#include <libluna/Input/XboxOneGamepadDevice.hpp>
#include <libluna/Vector.hpp>

namespace Luna::Input {
  class GenericGamepadDevice {
    public:
    using GamepadDeviceVariant = std::variant<
      Ps5GamepadDevice,
      Xbox360GamepadDevice,
      SwitchProGamepadDevice,
      XboxOneGamepadDevice
    >;

    GenericGamepadDevice(const GamepadDeviceVariant& device);

    bool isDown(GenericGamepad::Button button) const;
    float getAxis(GenericGamepad::Axis axis) const;
    Vector2f getStick(GenericGamepad::Stick stick) const;

    private:
    GamepadDeviceVariant mDevice;
  };
}
