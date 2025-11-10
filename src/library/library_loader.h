#pragma once

#include <engine/windows/command_line.h>

#include <expected>
#include <filesystem>
#include <functional>
#include <string>
#include <windows.h>

namespace library {

	class Library {
	public:
		virtual ~Library() {}
		virtual std::expected<void, std::string> load_functions(HINSTANCE library_handle) = 0;
	};

	class LibraryLoader {
	public:
		LibraryLoader(std::string library_name, std::string build_command);

		std::expected<void, std::string> load_library(Library* library);
		std::expected<void, std::string> trigger_hot_reload();
		std::expected<void, std::string> update_hot_reloading(Library* library, std::function<void()> on_library_unload, std::function<void()> on_library_reload);

	private:
		bool _library_file_has_changed() const;

		const std::string m_library_file_name;
		const std::string m_library_copy_file_name;
		const std::string m_library_build_command;
		std::filesystem::path m_library_path;
		std::filesystem::path m_library_copy_path;
		std::filesystem::file_time_type m_library_timestamp;
		HINSTANCE m_library_handle = {};
		engine::CommandProcess m_build_process = {};
	};

} // namespace library
