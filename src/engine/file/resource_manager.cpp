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
					.data = new RGBA[4],
				},
			}
		);
		m_images[0].data[0] = RGBA::black();
		m_images[0].data[1] = RGBA::purple();
		m_images[0].data[2] = RGBA::purple();
		m_images[0].data[3] = RGBA::black();
	}

	void ResourceManager::free_resources() {
		for (auto& [id, image] : m_images) {
			if (id > 0) {
				engine::free_image(image);
			}
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
		auto it = m_images.find(id.value);
		if (it == m_images.end()) {
			DEBUG_FAIL("Trying to access non-existing image with id %d", id.value);
			return m_images.at(INVALID_IMAGE_ID.value);
		}
		return it->second;
	}

} // namespace engine
