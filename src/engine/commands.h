#pragma once

#include <engine/audio/audio_id.h>

#include <variant>

namespace engine {

	// app
	struct AppCommand_Quit {};
	struct AppCommand_ToggleFullscreen {};
	using AppCommand = std::variant<AppCommand_Quit, AppCommand_ToggleFullscreen>;

	// audio
	struct AudioCommand_PlaySound {
		AudioID id;
	};
	using AudioCommand = std::variant<AudioCommand_PlaySound>;

	using Command = std::variant<AppCommand, AudioCommand>;

} // namespace engine
