#include <engine/file/resource_manager.h>

#include "resource_manager.h"
#include <engine/debug/assert.h>

namespace engine {

	ResourceManager::ResourceManager() {
		m_images.insert(
			{
				INVALID_IMAGE_ID.value,
				Image {
					.width = 2,
					.height = 2,
					.data = {
						RGBA::black(),
						RGBA::purple(),
						RGBA::purple(),
						RGBA::black(),
					},
				},
			}
		);
	}

	std::optional<ImageID> ResourceManager::load_image(std::filesystem::path filepath) {
		/* Check if already loaded */
		if (auto it = m_image_ids.find(filepath); it != m_image_ids.end()) {
			return ImageID(it->second);
		}

		/* Load and store image */
		if (std::optional<Image> image = Image::from_path(filepath)) {
			ImageID id = ImageID(m_next_image_id++);
			m_images[id.value] = image.value();
			return id;
		}

		/* Couldn't load image */
		return {};
	}

	std::optional<FontID> ResourceManager::load_font(std::filesystem::path filepath) {
		/* Check if already loaded */
		if (auto it = m_font_ids.find(filepath); it != m_font_ids.end()) {
			return FontID(it->second);
		}

		/* Load and store font */
		if (std::optional<Typeface> font = Typeface::from_path(filepath)) {
			FontID id = FontID(m_next_image_id++);
			m_fonts.push_back({ id.value, std::move(font.value()) });
			return id;
		}

		/* Couldn't load font */
		return {};
	}

	const Image& ResourceManager::image(ImageID id) const {
		auto it = m_images.find(id.value);
		if (it == m_images.end()) {
			DEBUG_FAIL("Trying to access non-existing image with id %d", id.value);
			return m_images.at(INVALID_IMAGE_ID.value);
		}
		return it->second;
	}

	Typeface& ResourceManager::font(FontID id) {
		auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [id](std::pair<int, Typeface> id_font) { return id.value == id_font.first; });
		if (it == m_fonts.end()) {
			DEBUG_FAIL("Trying to access non-existing font with id %d", id.value);
		}
		return it->second;
	}

} // namespace engine
