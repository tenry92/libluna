#pragma once

namespace Luna::Input {
  class Xbox360Gamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,

      kBack,
      kStart,
      kGuide,

      kLeftBumper,
      kRightBumper,
      kLeftTrigger,
      kRightTrigger,

      kLeftStickButton,
      kRightStickButton,

      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
    };

    enum Axis {
      kLeftStickX,
      kLeftStickY,
      kRightStickX,
      kRightStickY,
      kLeftTriggerAxis,
      kRightTriggerAxis,
    };

    enum Stick {
      kLeftStick,
      kRightStick,
    };
  };
}
