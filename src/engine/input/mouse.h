#pragma once

#include <engine/input/button.h>

#include <stdint.h>

namespace engine {

	struct Mouse {
		int16_t x;
		int16_t y;
        int16_t mouse_wheel_delta;
		Button left_button;
		Button right_button;
		Button middle_button;
		Button x1_button;
		Button x2_button;
	};

	void update_mouse(Mouse* mouse, int16_t mouse_wheel_delta);

} // namespace engine
