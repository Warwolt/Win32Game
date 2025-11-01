#pragma once

#include <string>
#include <variant>
#include <vector>

namespace engine {

	class ResourceManager;
	class SceneManager;
	class Window;

	class CommandList {
	public:
		void run_commands(bool* should_quit, Window* window, ResourceManager* resources, SceneManager* scene_manager);

		void quit();
		void toggle_fullscreen();
		void set_window_title(std::string window_title);
		void load_scene(std::string scene_name);

	private:
		struct Command_Quit {};
		struct Command_ToggleFullscreen {};
		struct Command_SetWindowTitle {
			std::string window_title;
		};
		struct Command_LoadScene {
			std::string scene_name;
		};
		using Command = std::variant<
			Command_Quit,
			Command_ToggleFullscreen,
			Command_SetWindowTitle,
			Command_LoadScene>;

		std::vector<Command> m_commands;
	};

} // namespace engine
