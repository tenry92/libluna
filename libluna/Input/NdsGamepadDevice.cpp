#include <libluna/Input/NdsGamepadDevice.hpp>
#include <libluna/Input/NdsGamepad.hpp>
#include <libluna/Input/NdsGamepadState.hpp>

using namespace Luna;
using namespace Luna::Input;

NdsGamepadDevice::NdsGamepadDevice(const NdsGamepadState* state)
  : mState(state) {}

bool NdsGamepadDevice::isDown(NdsGamepad::Button button) const {
  return mState && mState->isDown(button);
}
