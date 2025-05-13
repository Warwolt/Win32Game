#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>

namespace engine {

	struct InputDevices {
		Gamepad gamepad;
		Keyboard keyboard;
	};

	void update_input_devices(InputDevices* input);

} // namespace engine
