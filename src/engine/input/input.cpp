#include <engine/input/input.h>

namespace engine {

	void update_input_devices(InputDevices* input, const MouseEvents& mouse_events) {
		engine::update_gamepad(&input->gamepad);
		engine::update_mouse(&input->mouse, mouse_events);
		input->keyboard.update();
	}

} // namespace engine
