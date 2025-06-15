#include <engine/file/resource_manager.h>

#include <engine/debug/assert.h>

namespace engine {

	ResourceManager::ResourceManager() {
		m_missing_texture = Image {
			.width = 2,
			.height = 2,
			.data = {
				RGBA::black(),
				RGBA::purple(),
				RGBA::purple(),
				RGBA::black(),
			},
		};
	}

	std::optional<ImageID> ResourceManager::load_image(std::filesystem::path filepath) {
		/* Check if already loaded */
		if (auto cached_id = m_images.resource_id(filepath)) {
			return ImageID(cached_id.value());
		}

		/* Load and store image */
		if (std::optional<Image> image = Image::from_path(filepath)) {
			int id = m_images.add(image.value(), filepath);
			return ImageID(id);
		}

		/* Couldn't load image */
		LOG_ERROR("Couldn't load image with path \"%s\"", filepath.string().c_str());
		return {};
	}

	const Image& ResourceManager::image(ImageID id) const {
		if (const Image* image = m_images.try_get(id.value)) {
			return *image;
		}
		return m_missing_texture;
	}

} // namespace engine
