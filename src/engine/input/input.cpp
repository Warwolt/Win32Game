#include <engine/input/input.h>

namespace engine {

	void update_input_devices(InputDevices* input, const InputEvents& events) {
		engine::update_gamepad(&input->gamepad);
		engine::update_mouse(&input->mouse, events.mouse);
		input->keyboard.update();
	}

} // namespace engine
