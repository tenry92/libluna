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
      Input::KeyboardDevice,
      Input::Ps5GamepadDevice,
      Input::Xbox360GamepadDevice,
      Input::SwitchProGamepadDevice,
      Input::XboxOneGamepadDevice
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
    Input::KeyboardDevice getKeyboard() const;

    bool isPs5Gamepad() const;
    Input::Ps5GamepadDevice getPs5Gamepad() const;

    bool isXbox360Gamepad() const;
    Input::Xbox360GamepadDevice getXbox360Gamepad() const;

    bool isSwitchProGamepad() const;
    Input::SwitchProGamepadDevice getSwitchProGamepad() const;

    bool isXboxOneGamepad() const;
    Input::XboxOneGamepadDevice getXboxOneGamepad() const;

    bool isGamepad() const;
    Input::GenericGamepadDevice getGenericGamepad() const;
#endif

    private:
    InputDeviceVariant mDevice;
    int mIndex;
  };
}
