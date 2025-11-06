#include <engine/file/file.h>

#include <fstream>

namespace engine {

	std::optional<std::string> read_string_from_file(std::filesystem::path path) {
		std::ifstream file(path);
		if (!file.is_open()) {
			return {};
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	bool write_string_to_file(const std::string& str, std::filesystem::path path) {
		std::ofstream file(path);
		if (!file.is_open()) {
			return false;
		}
		file << str;
		return true;
	}

} // namespace engine
