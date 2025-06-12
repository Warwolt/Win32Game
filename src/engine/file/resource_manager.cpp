#include <engine/file/resource_manager.h>

namespace engine {

	ResourceManager::~ResourceManager() {
		for (auto& [id, image] : m_images) {
            engine::free_image(image);
		}
	}

	std::optional<ImageID> ResourceManager::load_image(const char* filepath) {
		/* Check if already loaded */
		if (auto it = m_image_ids.find(filepath); it != m_image_ids.end()) {
			return ImageID(it->second);
		}

		/* Load and store image */
		if (std::optional<Image> image = engine::load_image(filepath)) {
			ImageID id = ImageID(m_next_image_id++);
			m_images[id.value] = image.value();
			return id;
		}

		/* Couldn't load image */
		return {};
	}

	const Image& ResourceManager::image(ImageID id) const {
		// FIXME: debug assert that there's anything here
		return m_images.at(id.value);
	}

} // namespace engine
