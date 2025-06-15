#pragma once

#include <engine/graphics/image.h>
#include <engine/graphics/image_id.h>

#include <optional>
#include <unordered_map>

namespace engine {

	class ResourceManager {
	public:
		ResourceManager();

		std::optional<ImageID> load_image(const char* filepath);
		const Image& image(ImageID id) const;

	private:
		int m_next_image_id = 1;
		std::unordered_map<const char*, int> m_image_ids;
		std::unordered_map<int, Image> m_images;
	};

} // namespace engine
