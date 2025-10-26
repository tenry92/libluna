#pragma once

namespace Luna::Input {
  /**
   * @brief Provides enum definitions for a generic gamepad layout.
   *
   * This device acts as an adapter to specific gamepad implementations.
   *
   * @ingroup input
   */
  class GenericGamepad {
    public:
    enum Button {
      kSouth,   // Xbox: A, PlayStation: Cross, Nintendo: B
      kEast,    // Xbox: B, PlayStation: Circle, Nintendo: A
      kWest,    // Xbox: X, PlayStation: Square, Nintendo: Y
      kNorth,   // Xbox: Y, PlayStation: Triangle, Nintendo: X

      kBack,    // Xbox: View, PlayStation: Share, Nintendo: Minus
      kStart,   // Xbox: Menu, PlayStation: Options, Nintendo: Plus
      kGuide,   // Xbox: Xbox, PlayStation: PS, Nintendo: Home

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
