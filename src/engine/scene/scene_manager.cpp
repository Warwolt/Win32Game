#include <engine/scene/scene_manager.h>

#include <engine/debug/logging.h>

namespace engine {

	void SceneManager::HOT_RELOAD_unregister_all_scenes() {
		m_scene_constructors.clear();
	}

	void SceneManager::HOT_RELOAD_patch_vtables() {
		if (m_current_scene) {
			if (std::unique_ptr<Scene> dummy_scene = _try_create_scene(m_current_scene_name)) {
				void** current_vfptr = (void**)m_current_scene.get();
				void** new_vfptr = (void**)dummy_scene.get();
				*current_vfptr = *new_vfptr;
			}
		}
	}

	void SceneManager::register_scene(std::string name, SceneConstructor constructor) {
		m_scene_constructors[name] = constructor;
	}

	std::optional<SceneManagerError> SceneManager::load_scene(const std::string& scene_name) {
		std::unique_ptr<Scene> scene = _try_create_scene(scene_name);
		if (!scene) {
			return SceneManagerError::InvalidSceneName;
		}
		m_current_scene = std::move(scene);
		m_current_scene_name = scene_name;
		return {};
	}

	Scene* SceneManager::current_scene() {
		return m_current_scene.get();
	}

	std::unique_ptr<Scene> SceneManager::_try_create_scene(const std::string& scene_name) {
		auto it = m_scene_constructors.find(scene_name);
		if (it == m_scene_constructors.end()) {
			return nullptr;
		}
		auto& scene_constructor = it->second;
		return scene_constructor();
	}

} // namespace engine
