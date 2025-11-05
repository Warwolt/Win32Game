#pragma once

#include <engine/scene/scene.h>

#include <optional>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace engine {

	enum class SceneManagerError {
		InvalidSceneName,
	};

	class SceneManager {
	public:
		using SceneConstructor = std::function<std::unique_ptr<Scene>()>;

		void register_scene(std::string name, SceneConstructor constructor);
		std::optional<SceneManagerError> load_scene(const std::string& scene_name);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
