#include <engine/engine.h>

#include <engine/container/match_variant.h>
#include <engine/debug/logging.h>
#include <engine/input/input.h>

#include <utility>

namespace engine {

	std::expected<Engine, EngineError> initialize(HINSTANCE instance, WNDPROC wnd_proc) {
		Engine engine = {};

		initialize_logging();
		initialize_gamepad_support();
		initialize_debug(&engine.debug, &engine.resources);

		/* Create window */
		constexpr int zoom = 2;
		IVec2 screen_resolution = IVec2 { 640, 480 };
		const char* window_title = "Win32Game";
		std::expected<Window, EngineError> window_result = Window::initialize(instance, wnd_proc, zoom * screen_resolution, window_title);
		if (!window_result) {
			return std::unexpected(window_result.error());
		}

		engine.window = window_result.value();
		engine.screen_resolution = screen_resolution;
		engine.bitmap = Bitmap::with_size(engine.screen_resolution.x, engine.screen_resolution.y);
		engine.audio = initialize_audio_player();

		return engine;
	}

	void update(Engine* engine) {
		/* Update engine */
		update_debug(&engine->debug, engine->input, &engine->commands);

		/* Process commands */
		run_commands(engine->commands, &engine->should_quit, &engine->window, &engine->audio);
		engine->commands.clear();
	}

	void draw(Engine* engine) {
		draw_debug(&engine->renderer, engine->debug, engine->screen_resolution);
	}

} // namespace engine
