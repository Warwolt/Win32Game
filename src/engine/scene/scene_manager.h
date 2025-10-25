#pragma once

#include <engine/scene/scene.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

namespace engine {

	struct SceneID {
		using value_type = int;
		value_type value;
		bool operator==(const SceneID& other) const = default;
	};

	constexpr SceneID INVALID_SCENE_ID = SceneID(0);

	enum class SceneManagerError {
		InvalidSceneId,
	};

	class SceneManager {
	public:
		SceneID register_scene(std::function<std::unique_ptr<Scene>()> scene_constructor);
		std::expected<void, SceneManagerError> load_scene(SceneID scene_id);
		Scene* current_scene();

	private:
		int m_next_id = 1;
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<SceneID::value_type, std::function<std::unique_ptr<Scene>()>> m_scene_constructors;
	};

} // namespace engine
