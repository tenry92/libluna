#pragma once

#include <variant>

#ifdef N64
#include <libluna/Input/N64GamepadDevice.hpp>
#elif defined(__3DS__)
#include <libluna/Input/N3dsGamepadDevice.hpp>
#elif defined(NDS)
#include <libluna/Input/NdsGamepadDevice.hpp>
#else
#include <libluna/Input/KeyboardDevice.hpp>
#include <libluna/Input/Ps5GamepadDevice.hpp>
#include <libluna/Input/Xbox360GamepadDevice.hpp>
#include <libluna/Input/SwitchProGamepadDevice.hpp>
#include <libluna/Input/XboxOneGamepadDevice.hpp>
#include <libluna/Input/GenericGamepadDevice.hpp>
#endif

#if !defined(N64) && !defined(__3DS__) && !defined(NDS)
#define LUNA_FLEXIBLE_INPUT
#endif

namespace Luna {
  using namespace Input;

  class InputDevice {
    public:
#ifdef N64
    using InputDeviceVariant = std::variant<N64GamepadDevice>;
#elif defined(__3DS__)
    using InputDeviceVariant = std::variant<N3dsGamepadDevice>;
#elif defined(NDS)
    using InputDeviceVariant = std::variant<NdsGamepadDevice>;
#else
    using InputDeviceVariant = std::variant<
      KeyboardDevice,
      Ps5GamepadDevice,
      Xbox360GamepadDevice,
      SwitchProGamepadDevice,
      XboxOneGamepadDevice
    >;
#endif

    InputDevice(const InputDeviceVariant& device, int index = 0);

    /**
     * @brief Get the index of the device (e.g. gamepad number).
     *
     * This roughly corresponds to the order in which the devices were
     * connected. When a device is disconnected and a different device is
     * connected, it may get the same index as the previous device.
     * 
     * 0 corresponds to the first device.
     */
    int getIndex() const;

#ifdef N64
    N64GamepadDevice getN64Gamepad();
#elif defined(__3DS__)
    N3dsGamepadDevice getN3dsGamepad();
#elif defined(NDS)
    NdsGamepadDevice getNdsGamepad();
#else
    bool isKeyboard() const;
    KeyboardDevice getKeyboard();

    bool isPs5Gamepad() const;
    Ps5GamepadDevice getPs5Gamepad();

    bool isXbox360Gamepad() const;
    Xbox360GamepadDevice getXbox360Gamepad();

    bool isSwitchProGamepad() const;
    SwitchProGamepadDevice getSwitchProGamepad();

    bool isXboxOneGamepad() const;
    XboxOneGamepadDevice getXboxOneGamepad();

    bool isGamepad();
    GenericGamepadDevice getGenericGamepad();
#endif

    private:
    InputDeviceVariant mDevice;
    int mIndex;
  };
}
