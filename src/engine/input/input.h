#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/input_bindings.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>
#include <engine/input/time.h>

namespace engine {

	class Window;

	struct InputEvents {
		bool window_is_focused;
		MouseEvents mouse;
	};

	struct Input {
		Gamepad gamepad;
		Keyboard keyboard;
		Mouse mouse;
		InputBindings bindings;
		Time time_now;
		Time time_delta;
	};

	void update_input(Input* input, const InputEvents& events, const Window& window);

} // namespace engine
