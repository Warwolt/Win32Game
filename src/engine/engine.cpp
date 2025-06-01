#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/input/input.h>

#include <format>

namespace engine {

	std::expected<EngineState, EngineError> initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
		EngineState engine = {};
		initialize_logging(LogLevel::Debug);
		initialize_gamepad_support();
		if (std::expected<Window, EngineError> window_result = initialize_window(instance, wnd_proc, window_title)) {
			engine.window = window_result.value();
		}
		else {
			return std::unexpected(window_result.error());
		}
		engine.audio = initialize_audio_player();
		return engine;
	}

	void update(EngineState* engine, const InputDevices& input) {
		/* Process commands */
		for (const AppCommand& command : engine->commands.app_commands()) {
			if (auto* quit = std::get_if<QuitCommand>(&command)) {
				engine->should_quit = true;
			}
		}
		for (const AudioCommand& command : engine->commands.audio_commands()) {
			if (auto* play_sound = std::get_if<PlaySoundCommand>(&command)) {
				engine->audio.play(play_sound->id);
			}
		}
		engine->commands.clear();

		/* Update engine */
		update_debug(&engine->debug, input);
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		draw_debug(renderer, engine.debug);
	}

} // namespace engine
