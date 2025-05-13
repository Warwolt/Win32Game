#pragma once

#include <engine/input/button.h>

#include <stdint.h>

namespace engine {

	struct Gamepad {
		bool is_connected;
		Button dpad_up;
		Button dpad_right;
		Button dpad_down;
		Button dpad_left;
		Button start_button;
		Button back_button;
		Button left_shoulder;
		Button right_shoulder;
		Button a_button;
		Button b_button;
		Button x_button;
		Button y_button;
		int16_t left_stick_x;
		int16_t left_stick_y;
	};

	void initialize_gamepad();
	void update_gamepad(Gamepad* gamepad, int index = 0);

} // namespace engine
