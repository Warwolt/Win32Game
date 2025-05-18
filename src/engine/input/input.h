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

	void update_input_devices(InputDevices* input, const MouseEvents& mouse_events);

} // namespace engine
