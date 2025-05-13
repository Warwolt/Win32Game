#include <engine/input/input.h>

namespace engine {

	void update_input_devices(InputDevices* input) {
		engine::update_gamepad(&input->gamepad);
		input->keyboard.update();
	}

} // namespace engine
