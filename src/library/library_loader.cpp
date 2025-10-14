#include <library/library_loader.h>

namespace library {

	LibraryLoader::LibraryLoader(std::string library_name, std::string build_command)
		: m_library_file_name(library_name + ".dll")
		, m_library_copy_file_name(library_name + "-copy.dll")
		, m_library_build_command(build_command) {
		/* Save paths for DLL */
		char executable_path[MAX_PATH];
		GetModuleFileNameA(NULL, executable_path, MAX_PATH);
		std::filesystem::path executable_dir = std::filesystem::path(executable_path).parent_path();
		m_library_path = executable_dir / m_library_file_name;
		m_library_copy_path = executable_dir / m_library_copy_file_name;
	}

	std::expected<void, std::string> LibraryLoader::load_library(Library* library) {
		printf("Loading %s\n", m_library_copy_file_name.c_str());

		/* Check that library exists */
		if (!std::filesystem::exists(m_library_path)) {
			return std::unexpected(std::format("Couldn't find library file \"{}\"", m_library_path.string()));
		}

		/* Save timestamp of library write */
		m_library_timestamp = std::filesystem::last_write_time(m_library_path);

		/* Copy library file */
		std::filesystem::copy(m_library_path, m_library_copy_path, std::filesystem::copy_options::overwrite_existing);

		/* Load library */
		m_library_handle = LoadLibraryA(m_library_copy_file_name.c_str());
		if (!m_library_handle) {
			return std::unexpected(std::format("Failed to load library \"{}\"", m_library_copy_file_name));
		}

		/* Load functions */
		return library->load_functions(m_library_handle);
	}

	std::expected<void, std::string> LibraryLoader::trigger_hot_reload() {
		printf("Running command \"%s\"\n", m_library_build_command.c_str());
		std::expected<engine::CommandProcess, std::string> start_result = engine::CommandProcess::run(m_library_build_command);
		if (!start_result) {
			return std::unexpected(std::format("Running command failed: {}", start_result.error()));
		}
		m_build_process = *start_result;
		return {};
	}

	std::expected<void, std::string> LibraryLoader::update_hot_reloading(Library* library) {
		if (m_build_process.is_running()) {
			std::expected<engine::CommandUpdateResult, std::string> update_result = m_build_process.update();

			/* Check errors */
			if (!update_result) {
				return std::unexpected(std::format("Build command failed: {}", update_result.error()));
			}

			/* Echo stdout out */
			for (const std::string& line : update_result->output) {
				printf("> %s", line.c_str());
			}

			/* Handle build completion */
			if (update_result->exit_code.has_value()) {
				/* Handle error */
				DWORD exit_code = update_result->exit_code.value();
				if (exit_code != 0) {
					return std::unexpected(std::format("Build command failed with exit code: {}", exit_code));
				}

				/* Reload if we actually did rebuild library */
				if (_library_file_has_changed()) {
					FreeLibrary(m_library_handle);
					std::expected<void, std::string> load_result = load_library(library);
					if (!load_result) {
						return std::unexpected(std::format("Loading library failed: {}", load_result.error()));
					}
				}
			}
		}

		return {};
	}

	bool LibraryLoader::is_rebuilding_library() const {
		return m_build_process.is_running();
	}

	bool LibraryLoader::_library_file_has_changed() const {
		std::filesystem::file_time_type write_time = std::filesystem::last_write_time(m_library_path);
		return write_time != m_library_timestamp;
	}

} // namespace library
