#include <libluna/Input/KeyboardDevice.hpp>
#include <libluna/Input/Keyboard.hpp>
#include <libluna/Input/KeyboardState.hpp>

using namespace Luna::Input;

KeyboardDevice::KeyboardDevice(const KeyboardState* state)
	: mState(state) {}

bool KeyboardDevice::isDown(Keyboard::Scancode scancode) const {
	return mState && mState->isDown(scancode);
}

bool KeyboardDevice::isDown(char keycode) const {
	return mState && mState->isDown(keycode);
}
