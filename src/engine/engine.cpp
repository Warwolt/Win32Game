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
		engine.test_scene_id = engine.scene_manager.register_scene([test_screen_page = engine_args.test_screen_page](ResourceManager* resources) {
			return std::make_unique<TestScreen>(resources, test_screen_page);
		});
		std::expected<void, SceneManagerError> load_result = engine.scene_manager.load_scene(engine.test_scene_id, &engine.resources);
		if (!load_result) {
			LOG_FATAL("Failed to load initial scene");
			return {};
		}

		return engine;
	}

	// TODO
	// Two scenes: "Menu Scene" and "Game Scene"
	// Menu scene has two screens: Main menu, Test screen
	//
	// At some point we might want some basic UI framework but for now we can
	// probably roll a very simple Menu Scene with a simple two element list for
	// "play" and "test" that you select with arrow keys + enter.
	//
	// Pressing "test" should show the Test Screen.
	// Pressing "play" should change current scene to the Game Scene.

	void update(Engine* engine, CommandList* commands) {
		CPUProfilingScope_Engine();

		/* Update scene */
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->update(engine->input);
		}

		/* Show CPU profiling information in window title */
		float avg_fps = 1.0f / engine->frame_timer.average_delta();
		const char* window_title = "Game";
		std::string window_title_with_fps = std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->set_window_title(window_title_with_fps);

		/* Process commands */
		commands->run(&engine->should_quit, &engine->window);
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->draw(&engine->renderer);
		}
	}

} // namespace engine
