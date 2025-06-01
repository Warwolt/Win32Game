#include <engine/engine.h>

#include <engine/debug/renderer_test_screen.h>
#include <engine/input/input.h>

namespace engine {

	void update(EngineState* engine, const InputDevices& input) {
		/* Process commands */
		for (const engine::Command& command : engine->commands.commands()) {
			if (auto* quit_command = std::get_if<engine::QuitCommand>(&command)) {
				engine->should_quit = true;
			}
		}
		engine->commands.clear();

		/* Update engine */
		engine->debug.renderer_test_screen.update(input);
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		engine.debug.renderer_test_screen.draw(renderer);
	}

} // namespace engine
