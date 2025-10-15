#pragma once

#include <string_view>

namespace engine {

	constexpr const char* filename_from_path(const char* path_str) {
		constexpr char path_separator = '\\';
		std::string_view path_view = path_str;
		return path_view.substr(path_view.find_last_of(path_separator) + 1).data();
	}

} // namespace engine
