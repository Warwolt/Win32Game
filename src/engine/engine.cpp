#include <engine/engine.h>

#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/input/input.h>
#include <engine/utility/string_utility.h>

#include <engine/ui/debug_screen/debug_screen.h>
#include <engine/ui/screen.h>

namespace game {

	class MainMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "MainMenu";

		MainMenu(engine::ResourceManager*) {
		}

		void update(const engine::Input& input, engine::CommandList* commands) {
			//
		}

		void draw(engine::Renderer* renderer) const {
			//
		}

	private:
	};

	// TODO:
	// - Screen interface, ScreenStack
	// - MainMenuScreen
	class MenuScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "MenuScene";
		MenuScene(engine::ResourceManager*) {
		}
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
	};

	void MenuScene::update(const engine::Input& input, engine::CommandList* commands) {
	}

	void MenuScene::draw(engine::Renderer* renderer) const {
	}

	class GameScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "GameScene";
		GameScene(engine::ResourceManager*) {
		}
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;
	};

	void GameScene::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene("MenuScene");
		}
	}

	void GameScene::draw(engine::Renderer* renderer) const {
		engine::IVec2 screen_resolution = renderer->screen_resolution();
		engine::Rect text_box = {
			.x = screen_resolution.x / 2 - 36,
			.y = screen_resolution.y / 4,
		};
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, text_box, engine::RGBA::white(), "Game Scene");
	}

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

		/* Register scenes */
		engine.scene_manager.register_scene<game::MenuScene>();
		engine.scene_manager.register_scene<game::GameScene>();

		std::expected<void, SceneManagerError> load_result = engine.scene_manager.load_scene("MenuScene", &engine.resources);
		if (!load_result) {
			LOG_FATAL("Failed to load initial scene");
			return {};
		}

		/* Register screens */
		engine.screen_stack.register_screen<game::MainMenu>();

		return engine;
	}

	void update(Engine* engine, CommandList* commands) {
		CPUProfilingScope_Engine();

		/* Update scene */
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->update(engine->input, commands);
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
		commands->run(&engine->should_quit, &engine->window, &engine->resources, &engine->scene_manager);
	}

	void draw(Engine* engine) {
		CPUProfilingScope_Engine();
		if (Scene* current_scene = engine->scene_manager.current_scene()) {
			current_scene->draw(&engine->renderer);
		}
	}

} // namespace engine
