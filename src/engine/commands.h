#pragma once

#include <string>
#include <variant>
#include <vector>

namespace engine {

	class ResourceManager;
	class SceneManager;
	class ScreenStack;
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
		void toggle_fullscreen();
		void set_window_title(std::string window_title);
		void load_scene(std::string scene_name);
		void push_screen(std::string screen_name);
		void pop_screen();

	private:
		struct Command_Quit {};
		struct Command_ToggleFullscreen {};
		struct Command_SetWindowTitle {
			std::string window_title;
		};
		struct Command_LoadScene {
			std::string scene_name;
		};
		struct Command_PushScreen {
			std::string screen_name;
		};
		struct Command_PopScreen {};
		using Command = std::variant<
			Command_Quit,
			Command_ToggleFullscreen,
			Command_SetWindowTitle,
			Command_LoadScene,
			Command_PushScreen,
			Command_PopScreen>;

		std::vector<Command> m_commands;
	};

} // namespace engine
