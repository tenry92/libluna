#pragma once

namespace Luna::Input {
  class N64Gamepad {
    public:
    enum Button {
      kA,
      kB,
      kZ,
      kStart,
      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
      kL,
      kR,
      kCUp,
      kCDown,
      kCLeft,
      kCRight,
    };

    enum Axis {
      kStickX,
      kStickY,
    };
  };
}
