#pragma once

#include <string>
#include <variant>
#include <vector>

namespace engine {

	class Window;

	// clang-format off
	struct AppCommand_Quit {};
	struct AppCommand_ToggleFullscreen {};
	struct AppCommand_SetWindowTitle { std::string window_title; };
	// clang-format on

	using AppCommand = std::variant<
		AppCommand_Quit,
		AppCommand_ToggleFullscreen,
		AppCommand_SetWindowTitle>;

	using Command = std::variant<AppCommand>;

	void run_commands(const std::vector<Command>& commands, bool* should_quit, Window* window);

} // namespace engine
