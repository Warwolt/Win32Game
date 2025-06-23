#include <engine/engine.h>

#include <engine/container/match_variant.h>
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
		update_debug(&engine->debug, input, &engine->commands);

		/* Process commands */
		for (const Command& command : engine->commands) {
			/* App commands */
			MATCH_VARIANT_IF(command, AppCommand) {
				MATCH_UNIT_CASE(AppCommand_Quit) {
					engine->should_quit = true;
				}
				MATCH_UNIT_CASE(AppCommand_ToggleFullscreen) {
					engine->window.toggle_fullscreen();
				}
				MATCH_CASE(AppCommand_SetWindowTitle, window_title) {
					engine->window.set_title(window_title);
				}
			}
			/* Audio commands */
			MATCH_VARIANT_IF(command, AudioCommand) {
				MATCH_CASE(AudioCommand_PlaySound, sound_id) {
					engine->audio.play(sound_id);
				}
			}
		}
		engine->commands.clear();
	}

	void draw(Renderer* renderer, EngineState* engine) {
		draw_debug(renderer, engine->debug, &engine->resources, engine->screen_resolution);
	}

} // namespace engine
