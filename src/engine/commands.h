#pragma once

#include <variant>
#include <vector>

namespace engine {

	struct QuitCommand {};

	using Command = std::variant<QuitCommand>;

	class CommandAPI {
	public:
		void quit();

        void clear();
		const std::vector<Command>& commands() const;

	private:
		std::vector<Command> m_commands;
	};

} // namespace engine
