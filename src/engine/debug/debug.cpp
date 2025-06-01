#include <engine/debug/debug.h>

namespace engine {

	void update_debug(DebugState* debug, const InputDevices& input, const Window& window) {
		debug->test_screens.rendering.update(input, window);
	}

	void draw_debug(Renderer* renderer, const DebugState& debug) {
		debug.test_screens.rendering.draw(renderer);
	}

} // namespace engine
