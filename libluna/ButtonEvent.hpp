#pragma once

#include <libluna/String.hpp>

namespace Luna {
  class ButtonEvent {
    public:
    ButtonEvent(const String &name, bool down = true);
    ButtonEvent(const ButtonEvent &other);
    ~ButtonEvent();

    String getName() const;
    bool isDown() const;
    bool isUp() const;

    private:
    String mName;
    bool mDown{false};
  };
} // namespace Luna
