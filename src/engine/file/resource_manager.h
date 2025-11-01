#pragma once

#include <engine/graphics/font.h>
#include <engine/graphics/font_id.h>
#include <engine/graphics/image.h>
#include <engine/graphics/image_id.h>

#include <filesystem>
#include <optional>
#include <unordered_map>

namespace engine {

	class ResourceManager {
	public:
		static std::optional<ResourceManager> initialize(std::filesystem::path default_font_path);
		ImageID load_image(std::filesystem::path filepath);
		FontID load_font(std::filesystem::path filepath);
		const Image& image(ImageID id) const;
		Font& font(FontID id);

	private:
		int m_next_image_id = 1;
		std::unordered_map<std::filesystem::path, int> m_image_ids;
		std::unordered_map<int, Image> m_images;

		int m_next_font_id = 2;
		std::unordered_map<std::filesystem::path, int> m_font_ids;
		std::unordered_map<int, Font> m_fonts;
	};

} // namespace engine
