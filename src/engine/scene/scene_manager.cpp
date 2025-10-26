#include <engine/scene/scene_manager.h>

namespace engine {

	SceneID SceneManager::register_scene(SceneConstructor scene_constructor) {
		SceneID id = SceneID(m_next_id++);
		m_scene_constructors[id.value] = scene_constructor;
		return id;
	}

	std::expected<void, SceneManagerError> SceneManager::load_scene(SceneID scene_id, ResourceManager* resources) {
		auto it = m_scene_constructors.find(scene_id.value);
		if (it == m_scene_constructors.end()) {
			return std::unexpected(SceneManagerError::InvalidSceneId);
		}
		auto& scene_constructor = it->second;
		m_active_scene = scene_constructor(resources);
		return {};
	}

	Scene* SceneManager::current_scene() {
		return m_active_scene.get();
	}

} // namespace engine
