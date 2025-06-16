#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/input/input.h>

namespace engine {

	std::expected<EngineState, EngineError> initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
		EngineState engine = {};

		initialize_logging(LogLevel::Debug);
		initialize_gamepad_support();
		initialize_debug(&engine.debug, &engine.resources);

		constexpr int zoom = 2;
		IVec2 screen_resolution = IVec2 { 640, 480 };
		if (std::expected<Window, EngineError> window_result = Window::initialize(instance, wnd_proc, zoom * screen_resolution, window_title)) {
			engine.window = window_result.value();
		}
		else {
			return std::unexpected(window_result.error());
		}
		engine.screen_resolution = screen_resolution;
		engine.bitmap = Bitmap::with_size(engine.screen_resolution.x, engine.screen_resolution.y);
		engine.audio = initialize_audio_player();

		return engine;
	}

	void update(EngineState* engine, const InputDevices& input) {
		/* Process commands */
		for (const AppCommand& command : engine->commands.app_commands()) {
			if (auto* quit = std::get_if<QuitCommand>(&command)) {
				engine->should_quit = true;
			}
			if (auto* toggle_fullscreen = std::get_if<ToggleFullscreenCommand>(&command)) {
				engine->window.toggle_fullscreen();
			}
		}
		for (const AudioCommand& command : engine->commands.audio_commands()) {
			if (auto* play_sound = std::get_if<PlaySoundCommand>(&command)) {
				engine->audio.play(play_sound->id);
			}
		}
		engine->commands.clear();

		/* Update engine */
		update_debug(&engine->debug, input, &engine->resources);
	}

	void draw(Renderer* renderer, const EngineState& engine) {
		draw_debug(renderer, engine.debug, engine.screen_resolution);
	}

} // namespace engine
