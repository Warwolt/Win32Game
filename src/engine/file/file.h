#include <filesystem>
#include <optional>
#include <string>

namespace engine {

	std::optional<std::string> read_string_from_file(std::filesystem::path path);
	bool write_string_to_file(const std::string& str, std::filesystem::path path);

} // namespace engine
