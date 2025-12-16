#include <engine/file/resource_manager.h>

#include <engine/debug/assert.h>
#include <engine/debug/logging.h>

namespace engine {

	std::optional<ResourceManager> ResourceManager::initialize(std::filesystem::path default_font_path) {
		ResourceManager resources;

		/* Load missing image texture */
		resources.m_images.insert(
			{
				INVALID_IMAGE_ID.value,
				Image {
					.width = 2,
					.height = 2,
					.pixels = {
						Color::black(),
						Color::purple(),
						Color::purple(),
						Color::black(),
					},
				},
			}
		);

		/* Load default font */
		if (std::optional<Typeface> typeface = Typeface::from_path(default_font_path)) {
			resources.m_typefaces[DEFAULT_FONT_ID.value] = typeface.value();
		}
		else {
			LOG_ERROR("Couldn't load default font from path \"%s\"", default_font_path.string().c_str());
			return {};
		}

		return resources;
	}

	ImageID ResourceManager::load_image(std::filesystem::path filepath) {
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
		return INVALID_IMAGE_ID;
	}

	FontID ResourceManager::load_font(std::filesystem::path filepath) {
		/* Check if already loaded */
		auto same_filepath = [filepath](const std::pair<std::filesystem::path, int>& path_id) { return filepath == path_id.first; };
		if (auto it = std::find_if(m_typeface_ids.begin(), m_typeface_ids.end(), same_filepath); it != m_typeface_ids.end()) {
			return FontID(it->second);
		}

		/* Load and store font */
		if (std::optional<Typeface> typeface = Typeface::from_path(filepath)) {
			FontID id = FontID(m_next_font_id++);
			m_typefaces[id.value] = typeface.value();
			return id;
		}

		/* Couldn't load font */
		return INVALID_FONT_ID;
	}

	const Image& ResourceManager::image(ImageID id) const {
		auto it = m_images.find(id.value);
		if (it == m_images.end()) {
			DEBUG_FAIL("Trying to access non-existing image using id %d", id.value);
			return m_images.at(INVALID_IMAGE_ID.value);
		}
		return it->second;
	}

	Typeface& ResourceManager::typeface(FontID id) {
		DEBUG_ASSERT(m_typefaces.contains(id.value), "Trying to access non-existing typeface using id %d", id.value);
		return m_typefaces.at(id.value);
	}

	const Typeface& ResourceManager::typeface(FontID id) const {
		DEBUG_ASSERT(m_typefaces.contains(id.value), "Trying to access non-existing typeface using id %d", id.value);
		return m_typefaces.at(id.value);
	}

} // namespace engine
