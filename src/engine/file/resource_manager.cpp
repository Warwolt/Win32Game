#include <engine/file/resource_manager.h>

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
		auto same_filepath = [filepath](const std::pair<std::filesystem::path, int>& path_id) { return filepath == path_id.first; };
		if (auto it = std::find_if(m_font_ids.begin(), m_font_ids.end(), same_filepath); it != m_font_ids.end()) {
			return FontID(it->second);
		}

		/* Load and store font */
		if (std::optional<Font> font = Font::from_path(filepath)) {
			FontID id = FontID(m_next_font_id++);
			m_fonts[id.value] = font.value();
			return id;
		}

		/* Couldn't load font */
		return {};
	}

	const Image& ResourceManager::image(ImageID id) const {
		auto it = m_images.find(id.value);
		if (it == m_images.end()) {
			DEBUG_FAIL("Trying to access non-existing image using id %d", id.value);
			return m_images.at(INVALID_IMAGE_ID.value);
		}
		return it->second;
	}

	Font& ResourceManager::font(FontID id) {
		DEBUG_ASSERT(m_fonts.contains(id.value), "Trying to access non-existing font using id %d", id.value);
		return m_fonts.at(id.value);
	}

} // namespace engine
