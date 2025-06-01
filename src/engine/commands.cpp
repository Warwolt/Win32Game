#include <engine/commands.h>

namespace engine {

	void CommandAPI::quit() {
		m_app_commands.push_back(QuitCommand {});
	}

	void CommandAPI::toggle_fullscreen() {
		m_app_commands.push_back(ToggleFullscreenCommand {});
	}

	void CommandAPI::play_sound(AudioID id) {
		m_audio_commands.push_back(PlaySoundCommand { id });
	}

	void CommandAPI::clear() {
		m_app_commands.clear();
		m_audio_commands.clear();
	}

	const std::vector<AppCommand>& CommandAPI::app_commands() const {
		return m_app_commands;
	}

	const std::vector<AudioCommand>& CommandAPI::audio_commands() const {
		return m_audio_commands;
	}

} // namespace engine
