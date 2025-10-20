#include <engine/utility/string_utility.h>

#include <iterator>
#include <sstream>

namespace engine {

	std::vector<std::string> split_string_into_words(const std::string& text) {
		std::stringstream ss(text);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		return std::vector<std::string>(begin, end);
	}

} // namespace engine
