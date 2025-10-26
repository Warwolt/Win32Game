#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/input/input.h>
#include <engine/utility/string_utility.h>

namespace engine {

	constexpr IVec2 NES_RESOLUTION = IVec2 { 256, 240 };

	struct EngineArgs {
		int test_screen_page = 0;
	};

	std::optional<int64_t> parse_numeric_arg(const std::string& string, const std::string& arg_string) {
		if (string_starts_with(string, arg_string)) {
			return parse_number(string.substr(arg_string.length()));
		}
		return {};
	}

	static EngineArgs parse_args(const std::vector<std::string>& args) {
		EngineArgs engine_args;

		for (const std::string& arg : args) {
			if (std::optional<int64_t> test_screen_page = parse_numeric_arg(arg, "--test-screen-page=")) {
				engine_args.test_screen_page = (int32_t)test_screen_page.value() - 1;
			}
		}

		return engine_args;
	}

	std::optional<Engine> initialize(const std::vector<std::string>& args, HINSTANCE instance, WNDPROC wnd_proc) {
		Engine engine = {};
		EngineArgs engine_args = parse_args(args);
		initialize_logging();

		/* Create window */
		constexpr int zoom = 2;
		IVec2 screen_resolution = NES_RESOLUTION;
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
		engine.renderer = Renderer::with_bitmap(screen_resolution.x, screen_resolution.y);
		initialize_gamepad_support();
		initialize_debug(&engine.debug, &engine.resources, engine_args.test_screen_page);

		return engine;
	}

	void update(Engine* engine, CommandList* commands) {
		CPUProfilingScope_Engine();

		/* Update engine */
		update_debug(&engine->debug, engine->input, commands);

		/* Process commands */
		commands->run(&engine->should_quit, &engine->window);
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();
		draw_debug(&engine->renderer, engine->debug);
	}

} // namespace engine
