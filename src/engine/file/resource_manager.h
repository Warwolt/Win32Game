#pragma once

#include <engine/graphics/image.h>
#include <engine/graphics/image_id.h>

#include <filesystem>
#include <optional>
#include <unordered_map>

namespace engine {

	template <typename Resource>
	class ResourceStore {
	public:
		int add(Resource resource, std::filesystem::path path) {
			int id = m_next_id++;
			m_ids[path] = id;
			m_resources[id] = resource;
			return id;
		}

		std::optional<int> resource_id(std::filesystem::path path) {
			auto it = m_ids.find(path);
			if (it == m_ids.end()) {
				return {};
			}
			return it->second;
		}

		const Resource* try_get(int id) const {
			auto it = m_resources.find(id);
			if (it == m_resources.end()) {
				return nullptr;
			}
			return &it->second;
		}

	private:
		int m_next_id = 1;
		std::unordered_map<std::filesystem::path, int> m_ids;
		std::unordered_map<int, Resource> m_resources;
	};

	class ResourceManager {
	public:
		ResourceManager();

		std::optional<ImageID> load_image(std::filesystem::path filepath);
		const Image& image(ImageID id) const;

	private:
		Image m_missing_texture;
		ResourceStore<Image> m_images;
	};

} // namespace engine
