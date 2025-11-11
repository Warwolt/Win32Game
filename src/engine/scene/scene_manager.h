#pragma once

#include <engine/scene/scene.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace engine {

	enum class SceneManagerError {
		InvalidSceneName,
	};

	class SceneManager {
	public:
		using SceneConstructor = std::function<std::unique_ptr<Scene>()>;

		void HOT_RELOAD_unregister_all_scenes();
		void HOT_RELOAD_patch_vtables();

		void register_scene(std::string name, SceneConstructor constructor);
		std::optional<SceneManagerError> load_scene(const std::string& scene_name);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> _try_create_scene(const std::string& scene_name);
		std::string m_current_scene_name;
		std::unique_ptr<Scene> m_current_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
