#include <engine/file/save_file.h>

namespace engine {

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

} // namespace engine
