#pragma once

#include <windows.h>

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace engine {

	struct CommandUpdateResult {
		std::optional<DWORD> exit_code; // nullopt if still running
		std::vector<std::string> output;
	};

	class CommandProcess {
	public:
		static std::expected<CommandProcess, std::string> run(const std::string& command);
		std::expected<CommandUpdateResult, std::string> update();
		bool is_running() const;

	private:
		bool m_is_running;
		PROCESS_INFORMATION m_process_info;
		HANDLE m_stdout_read;
	};

} // namespace engine
