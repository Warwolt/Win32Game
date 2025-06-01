#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>
#include <engine/math/ivec2.h>

namespace engine {

	class Window;

	struct InputEvents {
		bool window_is_focused;
		MouseEvents mouse;
	};

	struct InputDevices {
		Gamepad gamepad;
		Keyboard keyboard;
		Mouse mouse;
	};

	void update_input_devices(InputDevices* input, const InputEvents& events, const Window& window);

} // namespace engine
