#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/input/input.h>

namespace engine {

	std::optional<Engine> initialize(HINSTANCE instance, WNDPROC wnd_proc) {
		Engine engine = {};
		initialize_logging();

		/* Create window */
		constexpr int zoom = 2;
		IVec2 screen_resolution = IVec2 { 640, 480 };
		const char* window_title = "Win32Game";
		std::optional<Window> window = Window::initialize(instance, wnd_proc, zoom * screen_resolution, window_title);
		if (!window) {
			LOG_FATAL("Failed to create window when initializing engine");
			return {};
		}

		engine.window = window.value();
		std::optional<ResourceManager> resources = ResourceManager::initialize("assets/font/ModernDOS8x16.ttf");
		if (!resources) {
			LOG_FATAL("Failed to create resource manager when initializing engine");
			return {};
		}
		engine.resources = resources.value();
		engine.screen_resolution = screen_resolution;
		engine.bitmap = Bitmap::with_size(engine.screen_resolution.x, engine.screen_resolution.y);
		engine.audio = initialize_audio_player();
		initialize_gamepad_support();
		initialize_debug(&engine.debug, &engine.resources);

		return engine;
	}

	void update(Engine* engine) {
		CPUProfilingScope_Engine();

		/* Update engine */
		update_debug(&engine->debug, engine->input, &engine->commands);

		/* Process commands */
		run_commands(engine->commands, &engine->should_quit, &engine->window, &engine->audio);
		engine->commands.clear();
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();
		draw_debug(&engine->renderer, engine->debug, engine->screen_resolution);
	}

} // namespace engine
