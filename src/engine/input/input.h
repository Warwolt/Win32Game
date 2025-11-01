#pragma once

#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>

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
		Time time_now;
	};

	void update_input(Input* input, const InputEvents& events, const Window& window);

} // namespace engine
