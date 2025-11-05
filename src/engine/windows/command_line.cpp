#include <engine/windows/command_line.h>

#include <format>

namespace engine {

	std::expected<CommandProcess, std::string> CommandProcess::run(const std::string& command) {
		/* Setup pipes for stdout and stderr */
		HANDLE stdout_read = NULL;
		HANDLE stdout_write = NULL;
		SECURITY_ATTRIBUTES security_attributes = {
			.nLength = sizeof(SECURITY_ATTRIBUTES),
			.lpSecurityDescriptor = NULL,
			.bInheritHandle = TRUE
		};
		BOOL pipe_was_created = CreatePipe(&stdout_read, &stdout_write, &security_attributes, 0);
		if (!pipe_was_created) {
			return std::unexpected("CreatePipe failed");
		}

		/* Ensure the read handle to the pipe is not inherited. */
		SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);

		/* Create process */
		PROCESS_INFORMATION process_info = {};
		STARTUPINFOA startup_info = {
			.cb = sizeof(STARTUPINFOA),
			.dwFlags = STARTF_USESTDHANDLES,
			.hStdInput = NULL,
			.hStdOutput = stdout_write,
			.hStdError = stdout_write,
		};
		BOOL process_was_created = CreateProcessA(
			NULL, // lpApplicationName
			const_cast<LPSTR>(command.c_str()), // lpCommandLine
			NULL, // lpProcessAttributes
			NULL, // lpThreadAttributes
			TRUE, // bInheritHandles
			0, // dwCreationFlags
			NULL, // lpEnvironment
			NULL, // lpCurrentDirectory
			&startup_info, // lpStartupInfo
			&process_info // lpProcessInformation
		);
		if (!process_was_created) {
			return std::unexpected(std::format("CreateProcessA failed, error: {}", GetLastError()));
		}

		/* Close write end of the pipe in the parent process */
		CloseHandle(stdout_write);

		CommandProcess process;
		process.m_is_running = true;
		process.m_process_info = process_info;
		process.m_stdout_read = stdout_read;
		return process;
	}

	std::expected<CommandUpdateResult, std::string> CommandProcess::update() {
		/* Check process is actually running */
		if (!m_is_running) {
			return std::unexpected("Can't update command process that's not running");
		}

		/* Echo content of pipe */
		std::vector<std::string> output;
		CHAR buffer[4096];
		DWORD bytes_read;
		if (ReadFile(m_stdout_read, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
			buffer[bytes_read] = '\0';
			output.push_back(buffer);
		}

		/* Check exit code */
		DWORD exit_code;
		BOOL received_exit_code = GetExitCodeProcess(m_process_info.hProcess, &exit_code);
		if (!received_exit_code) {
			return std::unexpected(std::format("GetExitCodeProcess failed, error: {}", GetLastError()));
		}

		if (exit_code != STILL_ACTIVE) {
			m_is_running = false;
			CloseHandle(m_process_info.hProcess);
			CloseHandle(m_process_info.hThread);
		}

		return CommandUpdateResult {
			.exit_code = exit_code == STILL_ACTIVE ? std::nullopt : std::make_optional(exit_code),
			.output = output,
		};
	}

	bool CommandProcess::is_running() const {
		return m_is_running;
	}

} // namespace engine
