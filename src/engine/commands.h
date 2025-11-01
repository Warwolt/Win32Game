#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace engine {

	class ResourceManager;
	class SceneManager;
	class ScreenStack;
	class Scene;
	class Screen;
	class Window;

	class CommandList {
	public:
		void run_commands(
			bool* should_quit,
			Window* window,
			ResourceManager* resources,
			SceneManager* scene_manager,
			ScreenStack* screen_stack
		);

		void quit();

		/* Window */
		void toggle_fullscreen();
		void set_window_title(std::string window_title);

		/* SceneManager */
		template <typename SceenType>
		void register_scene() {
			static_assert(std::is_default_constructible<SceenType>::value, "SceenType must be default constructible");
			m_commands.push_back(Command_RegisterScene { SceenType::NAME, []() { return std::make_unique<SceenType>(); } });
		}
		void load_scene(std::string scene_name);

		/* ScreenStack */
		template <typename ScreenType>
		void register_screen() {
			static_assert(std::is_default_constructible<ScreenType>::value, "ScreenType must be default constructible");
			m_commands.push_back(Command_RegisterScreen { ScreenType::NAME, []() { return std::make_unique<ScreenType>(); } });
		}
		void push_screen(std::string screen_name);
		void pop_screen();

	private:
		struct Command_Quit {};
		/* Window */
		struct Command_ToggleFullscreen {};
		struct Command_SetWindowTitle {
			std::string window_title;
		};

		/* SceneManager */
		struct Command_RegisterScene {
			std::string scene_name;
			std::function<std::unique_ptr<Scene>()> scene_constructor;
		};
		struct Command_LoadScene {
			std::string scene_name;
		};

		/* ScreenStack */
		struct Command_RegisterScreen {
			std::string screen_name;
			std::function<std::unique_ptr<Screen>()> screen_constructor;
		};
		struct Command_PushScreen {
			std::string screen_name;
		};
		struct Command_PopScreen {};

		using Command = std::variant<
			Command_Quit,

			/* Window */
			Command_ToggleFullscreen,
			Command_SetWindowTitle,

			/* SceneManager */
			Command_RegisterScene,
			Command_LoadScene,

			/* ScreenStack */
			Command_RegisterScreen,
			Command_PushScreen,
			Command_PopScreen>;

		std::vector<Command> m_commands;
	};

} // namespace engine
