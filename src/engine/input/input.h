#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>

namespace engine {

	struct InputDevices {
		Gamepad gamepad;
		Keyboard keyboard;
		Mouse mouse;
	};

	void update_input_devices(InputDevices* input, int16_t mouse_wheel_delta);

} // namespace engine
