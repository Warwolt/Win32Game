#pragma once

#include <engine/scene/scene.h>

#include <expected>
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

		template <typename SceneType>
		void register_scene() {
			static_assert(std::is_default_constructible<SceneType>::value, "SceneType must be default constructible");
			m_scene_constructors[SceneType::NAME] = []() { return std::make_unique<SceneType>(); };
		}
		std::expected<void, SceneManagerError> load_scene(const std::string& scene_name);
		Scene* current_scene();

	private:
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<std::string, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
