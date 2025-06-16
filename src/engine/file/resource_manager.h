#pragma once

#include <engine/graphics/image.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/font.h>
#include <engine/graphics/font_id.h>
#include <engine/container/flat_map.h>

#include <optional>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <memory>

namespace engine {

	class ResourceManager {
	public:
		ResourceManager();

		std::optional<ImageID> load_image(std::filesystem::path filepath);
		// std::optional<FontID> load_font(std::filesystem::path filepath);
		const Image& image(ImageID id) const;
		// Typeface* font(FontID id);

	private:
		int m_next_image_id = 1;
		engine::flat_map<std::filesystem::path, int> m_image_ids;
		engine::flat_map<int, Image> m_images;

		// int m_next_font_id = 1;
		// std::vector<std::pair<std::filesystem::path, int>> m_font_ids;
		// std::vector<Typeface> m_fonts;
	};

} // namespace engine
