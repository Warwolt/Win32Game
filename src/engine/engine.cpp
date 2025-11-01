#include <engine/engine.h>

#include <engine/debug/assert.h>
#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/input/input.h>
#include <engine/utility/string_utility.h>

#include <engine/ui/screen.h>

namespace game {

	// TODO: start moving this to own files in game directory
	class MainMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "MainMenu";

		void initialize(engine::ResourceManager* /*resources*/, engine::CommandList* /*commands*/) {
			LOG_DEBUG("MainMenu::initialize");
		}

		void update(const engine::Input& /*input*/, engine::CommandList* /*commands*/) {
			//
		}

		void draw(engine::Renderer* renderer) const {
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 0 }, engine::RGBA::white(), "MainMenu");
		}

	private:
	};

} // namespace game

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
			current_scene->update(engine->input, commands);
		}

		/* Update current screen */
		if (Screen* top_screen = engine->screen_stack.top_screen()) {
			top_screen->update(engine->input, commands);
		}

		/* Show CPU profiling information in window title */
		float avg_fps = 1.0f / engine->frame_timer.average_delta();
		const char* window_title = "Game";
		std::string window_title_with_fps = std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->set_window_title(window_title_with_fps);

		/* Toggle fullscreen */
		if (engine->input.keyboard.key_was_pressed_now(VK_F11)) {
			commands->toggle_fullscreen();
		}

		/* Process commands */
		commands->run_commands(
			&engine->should_quit,
			&engine->window,
			&engine->resources,
			&engine->scene_manager,
			&engine->screen_stack
		);
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();

		/* Draw current scene */
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->draw(&engine->renderer);
		}

		/* Draw current ui screen */
		if (Screen* top_screen = engine->screen_stack.top_screen()) {
			top_screen->draw(&engine->renderer);
		}
	}

} // namespace engine
