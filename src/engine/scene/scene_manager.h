#pragma once

#include <engine/scene/scene.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

namespace engine {

	class ResourceManager;

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
		using SceneConstructor = std::function<std::unique_ptr<Scene>(ResourceManager*)>;

		SceneID register_scene(SceneConstructor scene_constructor);
		std::expected<void, SceneManagerError> load_scene(SceneID scene_id, ResourceManager* resources);
		Scene* current_scene();

	private:
		int m_next_id = 1;
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<SceneID::value_type, SceneConstructor> m_scene_constructors;
	};

} // namespace engine
