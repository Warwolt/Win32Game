#pragma once

#include <string>
#include <vector>
#include <optional>

namespace engine {

    bool string_starts_with(const std::string& string, const std::string& prefix);
    std::vector<std::string> split_string_into_words(const std::string& text);
    std::optional<int64_t> parse_number(const std::string& string);

} // namespace engine
