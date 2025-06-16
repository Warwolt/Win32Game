#pragma once

#include <engine/audio/audio_id.h>

#include <variant>

namespace engine {

	// app
	struct QuitCommand {};
	struct ToggleFullscreenCommand {};
	using AppCommand = std::variant<QuitCommand, ToggleFullscreenCommand>;

	// audio
	struct PlaySoundCommand {
		AudioID id;
	};
	using AudioCommand = std::variant<PlaySoundCommand>;

	using Command = std::variant<AppCommand, AudioCommand>;

} // namespace engine
