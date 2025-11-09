#pragma once

#include <expected>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace engine {

	enum class SaveFileError {
		InvalidJson,
	};

	class SaveFile {
	public:
		static std::expected<SaveFile, SaveFileError> from_json_string(std::string json_string);
		std::string to_json_string() const;

		bool contains(const std::string& key) const;
		const nlohmann::ordered_json& operator[](const std::string& key) const;
		nlohmann::ordered_json& operator[](const std::string& key);

		template <typename T>
		std::optional<T> try_get(const std::string& key) const {
			if (m_json_data.contains(key)) {
				return m_json_data[key].get<T>();
			}
			return {};
		}

	private:
		nlohmann::ordered_json m_json_data;
	};

} // namespace engine
