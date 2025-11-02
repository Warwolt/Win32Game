#include <engine/input/input.h>

namespace engine {

	void update_input(Input* input, const InputEvents& events, const Window& window) {
		engine::update_gamepad(&input->gamepad);
		engine::update_mouse(&input->mouse, events.mouse, window);
		input->keyboard.update();

		const Time time_now = Time::now();
		input->time_delta = time_now - input->time_now;
		input->time_now = time_now;
	}

} // namespace engine
