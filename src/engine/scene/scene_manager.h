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

		void reset();

		void register_scene(std::string name, SceneConstructor constructor);
		std::optional<SceneManagerError> load_scene(const std::string& scene_name);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> m_current_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
