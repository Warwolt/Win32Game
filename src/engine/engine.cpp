#include <engine/engine.h>

#include <engine/debug/renderer_test_screen.h>
#include <engine/input/input.h>

namespace engine {

	void update(EngineState* engine, const InputDevices& input) {
		engine->debug.renderer_test_screen.update(input);
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		engine.debug.renderer_test_screen.draw(renderer);
	}

} // namespace engine
