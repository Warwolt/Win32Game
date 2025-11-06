#pragma once

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace engine {

	enum class SaveFileError {
		InvalidJson,
	};

	class SaveFile {
	public:
		static std::expected<SaveFile, SaveFileError> from_json_string(std::string json_string);
		std::string to_json_string();

		const nlohmann::ordered_json& operator[](std::string key) const;
		nlohmann::ordered_json& operator[](std::string key);

	private:
		nlohmann::ordered_json m_json_data;
	};

} // namespace engine
