#include <engine/file/save_file.h>

namespace engine {

	SaveFileValue::SaveFileValue(nlohmann::json data)
		: m_json_data(data) {
	}

	bool SaveFileValue::is_null() const {
		return m_json_data.is_null();
	}

	bool SaveFileValue::is_boolean() const {
		return m_json_data.is_boolean();
	}

	bool SaveFileValue::is_number() const {
		return m_json_data.is_number();
	}

	bool SaveFileValue::is_string() const {
		return m_json_data.is_string();
	}

	std::optional<bool> SaveFileValue::as_boolean() const {
		if (m_json_data.is_boolean()) {
			return m_json_data.get<bool>();
		}
		return {};
	}

	std::optional<double> SaveFileValue::as_number() const {
		if (m_json_data.is_number()) {
			return m_json_data.get<double>();
		}
		return {};
	}

	std::optional<std::string> SaveFileValue::as_string() const {
		if (m_json_data.is_string()) {
			return m_json_data.get<std::string>();
		}
		return {};
	}

	std::expected<SaveFile, SaveFileError> SaveFile::from_json_string(std::string json_string) {
		SaveFile save_file;
		try {
			save_file.m_json_data = nlohmann::json::parse(json_string);
		}
		catch (nlohmann::json::exception) {
			return std::unexpected(SaveFileError::InvalidJson);
		}
		return save_file;
	}

	SaveFileValue SaveFile::operator[](std::string key) const {
		auto it = m_json_data.find(key);
		if (it == m_json_data.end()) {
			return SaveFileValue(); // null
		}
		return *it;
	}

} // namespace engine
