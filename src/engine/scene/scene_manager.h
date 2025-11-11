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

		template <typename SceneType>
		void register_scene() {
			static_assert(std::is_default_constructible<SceneType>::value, "SceneType must be default constructible");
			m_scene_constructors[SceneType::NAME] = +[]() { return std::make_unique<SceneType>(); };
		}

		std::optional<SceneManagerError> load_scene(const std::string& scene_name);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> _try_create_scene(const std::string& scene_name);
		std::string m_current_scene_name;
		std::unique_ptr<Scene> m_current_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
