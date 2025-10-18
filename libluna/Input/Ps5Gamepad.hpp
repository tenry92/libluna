#pragma once

namespace Luna::Input {
  class Ps5Gamepad {
    public:
    enum Button {
      kCross,
      kCircle,
      kSquare,
      kTriangle,

      kCreate,
      kOptions,
      kPS,
      kTouchpad,

      kL1,
      kR1,
      kL2,
      kR2,

      kLeftStickButton,
      kRightStickButton,

      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,

      kMute,     // Mute
    };

    enum Axis {
      kLeftStickX,
      kLeftStickY,
      kRightStickX,
      kRightStickY,
      kL2Axis,
      kR2Axis,
    };

    enum Stick {
      kLeftStick,
      kRightStick,
    };
  };
}
