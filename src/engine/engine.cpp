#include <engine/engine.h>

#include <engine/debug/assert.h>
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

	void on_dll_unload(Engine* engine) {
		LOG_DEBUG("DLL unloading, unloading current scene");
		engine->scene_manager.unload_scene();
	}

	void on_dll_reloaded(Engine* engine) {
		// 2025-11-10
		// Speculative fix for a crash occuring on hot-reload, because we seem
		// to call into a Scene with invalid v-table after reloading.
		// By reconstructing the Scene and Screen we make sure the v-table they
		// are using are valid.
		LOG_INFO("DLL reloaded, reloading current scene and screen");
		CommandList commands;
		engine->scene_manager.reload_last_scene();
		engine->scene_manager.current_scene()->initialize(engine->game_data, &engine->resources, &commands);
		commands.run_commands(engine);
	}

	std::optional<Engine> initialize(const std::vector<std::string>& args, HINSTANCE instance, WNDPROC wnd_proc, game::GameData* game_data) {
		Engine engine = {};
		engine.game_data = game_data;
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

		/* Initialize subsystems */
		engine.window = window.value();
		std::optional<ResourceManager> resources = ResourceManager::initialize("assets/font/ModernDOS8x16.ttf");
		if (!resources) {
			LOG_FATAL("Failed to create resource manager when initializing engine");
			return {};
		}
		engine.resources = resources.value();
		engine.renderer = Renderer::with_bitmap(screen_resolution.x, screen_resolution.y);
		initialize_gamepad_support();

		return engine;
	}

	void update(Engine* engine, CommandList* commands) {
		CPUProfilingScope_Engine();

		/* Update current scene */
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->update(engine->game_data, engine->input, commands);
		}

		/* Update top-most screen */
		if (Screen* top_screen = engine->screen_stack.top_screen()) {
			top_screen->update(engine->game_data, engine->input, commands);
		}

		/* Show CPU profiling information in window title */
		float avg_fps = 1.0f / engine->frame_timer.average_delta();
		const char* window_title = "Game";
		std::string window_title_with_fps = std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->set_window_title(window_title_with_fps);

		/* Process commands */
		commands->run_commands(engine);
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();

		/* Draw current scene */
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->draw(*engine->game_data, &engine->renderer);
		}

		/* Draw current ui screen */
		if (Screen* top_screen = engine->screen_stack.top_screen()) {
			top_screen->draw(*engine->game_data, &engine->renderer);
		}
	}

} // namespace engine
