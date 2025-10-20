#include <engine/utility/string_utility.h>

#include <cstring>
#include <iterator>
#include <sstream>

namespace engine {

	bool string_starts_with(const std::string& string, const std::string& prefix) {
		return strncmp(prefix.c_str(), string.c_str(), prefix.length()) == 0;
	}

	std::vector<std::string> split_string_into_words(const std::string& text) {
		std::stringstream ss(text);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		return std::vector<std::string>(begin, end);
	}

	std::optional<int64_t> parse_number(const std::string& string) {
		errno = 0;
		char* end = nullptr;
		int64_t number = strtol(string.c_str(), &end, 10);
		if (errno == ERANGE || end == string.c_str() || *end != '\0') {
			return {};
		}
		return number;
	}

} // namespace engine
