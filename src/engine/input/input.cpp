#include <engine/input/input.h>

namespace engine {

	void update_input(Input* input, const InputEvents& events, const Window& window) {
		engine::update_gamepad(&input->gamepad);
		engine::update_mouse(&input->mouse, events.mouse, window);
		input->keyboard.update();
		input->time_now = Time::now();
	}

} // namespace engine
