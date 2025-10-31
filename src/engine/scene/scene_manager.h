#pragma once

#include <engine/scene/scene.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

namespace engine {

	class ResourceManager;

	enum class SceneManagerError {
		InvalidSceneName,
	};

	class SceneManager {
	public:
		using SceneConstructor = std::function<std::unique_ptr<Scene>(ResourceManager*)>;

		template <typename SceneType>
		void register_scene() {
			static_assert(std::constructible_from<SceneType, ResourceManager*>, "Scene must have a constructor that accepts a ResourceManager, but none was found.");
			m_scene_constructors[SceneType::NAME] = [](ResourceManager* resources) { return std::make_unique<SceneType>(resources); };
		}

		std::expected<void, SceneManagerError> load_scene(const std::string& scene_name, ResourceManager* resources);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
