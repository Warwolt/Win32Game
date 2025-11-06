#pragma once

#include <expected>
#include <string>

#include <nlohmann/json.hpp>

namespace engine {

	class SaveFileValue {
	public:
		// basically just wrapper around a nlohmann::json value

		// void set(int number);
		// void set(bool boolean);
		// void set(std::string string);

		// bool is_null() const;
		// bool is_boolean() const;
		// bool is_number() const;
		// bool is_string() const;

		// std::optional<bool> as_boolean() const;
		// std::optional<int> as_number() const;
		// std::optional<std::string> as_string() const;

	private:
		// json data
	};

	enum class SaveFileError {
		InvalidJson,
	};

	class SaveFile {
	public:
		static std::expected<SaveFile, SaveFileError> from_json_string(std::string json_string);
		// std::string to_json_string(); // for serialization
		// std::optional<SaveFileValue> operator[](std::string key) const;

	private:
		nlohmann::json m_json_data;
	};

} // namespace engine
