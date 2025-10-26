#include <engine/scene/scene_manager.h>

namespace engine {

	void SceneManager::register_scene(std::string scene_name, SceneConstructor scene_constructor) {
		m_scene_constructors[scene_name] = scene_constructor;
	}

	std::expected<void, SceneManagerError> SceneManager::load_scene(const std::string& scene_name, ResourceManager* resources) {
		auto it = m_scene_constructors.find(scene_name);
		if (it == m_scene_constructors.end()) {
			return std::unexpected(SceneManagerError::InvalidSceneName);
		}
		auto& scene_constructor = it->second;
		m_active_scene = scene_constructor(resources);
		return {};
	}

	Scene* SceneManager::current_scene() {
		return m_active_scene.get();
	}

} // namespace engine
