#include <engine/engine.h>

#include <engine/input/input.h>

namespace engine {

	void update(EngineState* engine, const InputDevices& input) {
		engine->debug.renderer_test.window_size = input.window_size;
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		draw_renderer_test_screen(renderer, engine.debug.renderer_test);
	}

} // namespace engine
