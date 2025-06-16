#include <engine/debug/debug.h>

#include <engine/graphics/window.h>

namespace engine {

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->test_screens.rendering.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input, Window* window, ResourceManager* resources) {
		debug->test_screens.rendering.update(input, resources);
		// FIXME: We should replace "Game" with some constant
		window->set_title(std::format("{} ({:.1f} fps)", "Game", 1.0f / debug->performance.frame_timer.average_delta()));
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution) {
		debug.test_screens.rendering.draw(renderer, screen_resolution);
	}

} // namespace engine
