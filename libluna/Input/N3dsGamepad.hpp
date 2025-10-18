#pragma once

namespace Luna::Input {
  class N3dsGamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,
      kL,
      kR,
      kZL,
      kZR,
      kStart,
      kSelect,
      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
      kHome,
      kPower,
    };

    enum Axis {
      kStickX,
      kStickY,
    };
  };
}
