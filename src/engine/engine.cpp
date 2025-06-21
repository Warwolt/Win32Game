#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/input/input.h>

#include <utility>

namespace engine {

	std::expected<EngineState, EngineError> initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
		EngineState engine = {};

		initialize_logging(LogLevel::Debug);
		initialize_gamepad_support();
		initialize_debug(&engine.debug, &engine.resources);

		constexpr int zoom = 2;
		IVec2 screen_resolution = IVec2 { 1920 / 4, 1200 / 4 };
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
		/* Update engine */
		update_debug(&engine->debug, input, &engine->window);

		/* Process commands */
		for (const Command& command : engine->commands) {
			/* App commands */
			if (auto* app_command = std::get_if<AppCommand>(&command)) {
				if (auto* quit = std::get_if<AppCommand_Quit>(app_command)) {
					engine->should_quit = true;
				}
				if (auto* toggle_fullscreen = std::get_if<AppCommand_ToggleFullscreen>(app_command)) {
					engine->window.toggle_fullscreen();
				}
			}
			/* Audio commands */
			if (auto* audio_command = std::get_if<AudioCommand>(&command)) {
				if (auto* play_sound = std::get_if<AudioCommand_PlaySound>(audio_command)) {
					engine->audio.play(play_sound->id);
				}
			}
		}
		engine->commands.clear();
	}

	void draw(Renderer* renderer, EngineState* engine) {
		draw_debug(renderer, engine->debug, &engine->resources, engine->screen_resolution);
	}

} // namespace engine
