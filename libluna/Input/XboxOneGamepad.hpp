#pragma once

namespace Luna::Input {
  class XboxOneGamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,

      kView,
      kMenu,
      kXbox,

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
