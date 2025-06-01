#pragma once

#include <engine/audio/audio_id.h>

#include <variant>
#include <vector>

namespace engine {

	// app
	struct QuitCommand {};

	// audio
	struct PlaySoundCommand {
		AudioID id;
	};

	using AppCommand = std::variant<QuitCommand>;
	using AudioCommand = std::variant<PlaySoundCommand>;

	class CommandAPI {
	public:
		// app
		void quit();

		// audio
		void play_sound(AudioID id);

		void clear();
		const std::vector<AppCommand>& app_commands() const;
		const std::vector<AudioCommand>& audio_commands() const;

	private:
		std::vector<AppCommand> m_app_commands;
		std::vector<AudioCommand> m_audio_commands;
	};

} // namespace engine
