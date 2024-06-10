#pragma once

#include <memory>

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
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
