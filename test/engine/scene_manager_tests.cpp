#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

namespace engine {

	struct Input;
	class Renderer;

	class Scene {
	public:
		virtual ~Scene() {};
		virtual void update(const Input& input) = 0;
		virtual void draw(Renderer* renderer) = 0;
	};

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
		SceneID register_scene(std::function<std::unique_ptr<Scene>()> scene_constructor) {
			SceneID id = SceneID(m_next_id++);
			m_scene_constructors[id.value] = scene_constructor;
			return id;
		}

		std::expected<void, SceneManagerError> load_scene(SceneID scene_id) {
			auto it = m_scene_constructors.find(scene_id.value);
			if (it == m_scene_constructors.end()) {
				return std::unexpected(SceneManagerError::InvalidSceneId);
			}
			auto& scene_constructor = it->second;
			m_active_scene = scene_constructor();
            return {};
		}

		Scene* current_scene() {
			return m_active_scene.get();
		}

	private:
		int m_next_id = 1;
		std::unique_ptr<Scene> m_active_scene;
		std::unordered_map<SceneID::value_type, std::function<std::unique_ptr<Scene>()>> m_scene_constructors;
	};

} // namespace engine

using namespace engine;

struct TestScene : public Scene {
	static int num_instances;
	TestScene() {
		num_instances++;
	}
	~TestScene() {
		num_instances--;
	}
	void update(const Input& /*input*/) {}
	void draw(Renderer* /*renderer*/) {}
};

int TestScene::num_instances = 0;

TEST(SceneManagerTests, InitiallyHoldsNoScene) {
	SceneManager scene_manager;

	Scene* current_scene = scene_manager.current_scene();

	EXPECT_EQ(current_scene, nullptr);
}

TEST(SceneManagerTests, RegisteredSceneCanBeLoaded) {
	SceneManager scene_manager;

	SceneID scene_id = scene_manager.register_scene([]() {
		return std::make_unique<TestScene>();
	});
	EXPECT_EQ(TestScene::num_instances, 0);

	std::expected<void, SceneManagerError> result = scene_manager.load_scene(scene_id);
    EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_NE(scene_manager.current_scene(), nullptr);
}

PARAMETERIZED_TEST(SceneManagerTests, LoadSceneWithBadID_GivesError, int, testing::Values(0,1)) {
	SceneManager scene_manager;
    SceneID scene_id = SceneID(GetParam());

	std::expected<void, SceneManagerError> result = scene_manager.load_scene(scene_id);

	ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SceneManagerError::InvalidSceneId);
}

// switch between 2 scenes
