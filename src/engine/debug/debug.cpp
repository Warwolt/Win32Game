#include <engine/debug/debug.h>

namespace engine {

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->test_screens.rendering.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input) {
		debug->test_screens.rendering.update(input);
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution) {
		debug.test_screens.rendering.draw(renderer, screen_resolution);
	}

} // namespace engine
