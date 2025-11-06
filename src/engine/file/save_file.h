#pragma once

#include <optional>
#include <expected>
#include <string>

#include <nlohmann/json.hpp>

namespace engine {

	class SaveFileValue {
	public:
		SaveFileValue() = default;
		SaveFileValue(nlohmann::json data);

		bool is_null() const;
		bool is_boolean() const;
		bool is_number() const;
		bool is_string() const;

		// operator=

		std::optional<bool> as_boolean() const;
		std::optional<double> as_number() const;
		std::optional<std::string> as_string() const;

	private:
		nlohmann::json m_json_data;
	};

	enum class SaveFileError {
		InvalidJson,
	};

	class SaveFile {
	public:
		static std::expected<SaveFile, SaveFileError> from_json_string(std::string json_string);
		// std::string to_json_string(); // for serialization
		SaveFileValue operator[](std::string key) const;

	private:
		nlohmann::json m_json_data;
	};

} // namespace engine
