#include <engine/input/input.h>

namespace engine {

	void update_input_devices(InputDevices* input, int16_t mouse_wheel_delta) {
		engine::update_gamepad(&input->gamepad);
		engine::update_mouse(&input->mouse, mouse_wheel_delta);
		input->keyboard.update();
	}

} // namespace engine
