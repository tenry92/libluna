#include <libluna/ButtonEvent.hpp>

using namespace Luna;

class ButtonEvent::impl {
  public:
  String mName;
  bool mDown{false};
};

ButtonEvent::ButtonEvent(const String &name, bool down)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mName = name;
  mImpl->mDown = down;
}

ButtonEvent::ButtonEvent(const ButtonEvent &other)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mName = other.mImpl->mName;
  mImpl->mDown = other.mImpl->mDown;
}

ButtonEvent::~ButtonEvent() = default;

String ButtonEvent::getName() const { return mImpl->mName; }

bool ButtonEvent::isDown() const { return mImpl->mDown; }

bool ButtonEvent::isUp() const { return !mImpl->mDown; }
