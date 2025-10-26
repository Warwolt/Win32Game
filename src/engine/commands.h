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
		void run(bool* should_quit, Window* window, ResourceManager* resources, SceneManager* scene_manager);

		void quit();
		void toggle_fullscreen();
		void set_window_title(std::string window_title);
		void load_scene(std::string scene_name);

	private:
		struct AppCommand_Quit {};
		struct AppCommand_ToggleFullscreen {};
		struct AppCommand_SetWindowTitle {
			std::string window_title;
		};
		struct AppCommand_LoadScene {
			std::string scene_name;
		};
		using AppCommand = std::variant<
			AppCommand_Quit,
			AppCommand_ToggleFullscreen,
			AppCommand_SetWindowTitle,
			AppCommand_LoadScene>;
		using Command = std::variant<AppCommand>;

		std::vector<Command> m_commands;
	};

} // namespace engine
