#include <libluna/ButtonEvent.hpp>

using namespace Luna;

ButtonEvent::ButtonEvent(const String &name, bool down)
    : mName{name}, mDown{down} {}

ButtonEvent::ButtonEvent(const ButtonEvent &other)
    : mName{other.mName}, mDown{other.mDown} {}

ButtonEvent::~ButtonEvent() = default;

String ButtonEvent::getName() const { return mName; }

bool ButtonEvent::isDown() const { return mDown; }

bool ButtonEvent::isUp() const { return !mDown; }
