#pragma once

#include <engine/audio/audio_id.h>

#include <string>
#include <variant>

namespace engine {

	// clang-format off
	struct AppCommand_Quit {};
	struct AppCommand_ToggleFullscreen {};
	struct AppCommand_SetWindowTitle { std::string window_title; };
	// clang-format on

	using AppCommand = std::variant<
		AppCommand_Quit,
		AppCommand_ToggleFullscreen,
		AppCommand_SetWindowTitle>;

	// clang-format off
	struct AudioCommand_PlaySound { AudioID id; };
	// clang-format on

	using AudioCommand = std::variant<
		AudioCommand_PlaySound>;

	using Command = std::variant<AppCommand, AudioCommand>;

} // namespace engine
