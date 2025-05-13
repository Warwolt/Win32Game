#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>

namespace engine {

	struct InputDevices {
		Gamepad gamepad;
		Keyboard keyboard;
	};

} // namespace engine
