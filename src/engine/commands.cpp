#include <engine/commands.h>

namespace engine {

	void CommandAPI::quit() {
		m_commands.push_back(QuitCommand {});
	}

    void CommandAPI::clear() {
        m_commands.clear();
    }

	const std::vector<Command>& CommandAPI::commands() const {
		return m_commands;
	}

} // namespace engine
