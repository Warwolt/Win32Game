#pragma once

#include <string>
#include <variant>
#include <vector>

namespace engine {

	class Window;

	class CommandList {
	public:
		void run(bool* should_quit, Window* window);

		void quit();
		void toggle_fullscreen();
		void set_window_title(std::string window_title);

	private:
		struct AppCommand_Quit {};
		struct AppCommand_ToggleFullscreen {};
		struct AppCommand_SetWindowTitle {
			std::string window_title;
		};
		using AppCommand = std::variant<
			AppCommand_Quit,
			AppCommand_ToggleFullscreen,
			AppCommand_SetWindowTitle>;
		using Command = std::variant<AppCommand>;

		std::vector<Command> m_commands;
	};

} // namespace engine
