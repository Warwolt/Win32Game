#include <engine/engine.h>

#include <engine/debug/test/rendering_tests.h>
#include <engine/input/input.h>

namespace engine {

	void update(EngineState* engine, const InputDevices& input) {
		/* Process commands */
		for (const engine::AppCommand& command : engine->commands.app_commands()) {
			if (auto* quit = std::get_if<engine::QuitCommand>(&command)) {
				engine->should_quit = true;
			}
		}
		for (const engine::AudioCommand& command : engine->commands.audio_commands()) {
			if (auto* play_sound = std::get_if<engine::PlaySoundCommand>(&command)) {
				engine->audio.play(play_sound->id);
			}
		}
		engine->commands.clear();

		/* Update engine */
		engine->debug.test_screens.rendering.update(input);
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		engine.debug.test_screens.rendering.draw(renderer);
	}

} // namespace engine
