#include <gtest/gtest.h>

#include <functional>
#include <unordered_map>
#include <memory>

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

	class SceneManager {
	public:
		using ScenePtr = std::unique_ptr<Scene>;

        SceneID register_scene(std::function<ScenePtr()> scene_constructor) {
            SceneID id = SceneID(m_next_id++);
            m_scene_constructors[id.value] = scene_constructor;
            return id;
        }

        void load_scene(SceneID scene_id) {
            if (auto it = m_scene_constructors.find(scene_id.value); it != m_scene_constructors.end()) {
                auto& scene_constructor = it->second;
                m_active_scene = scene_constructor();
            }
            // else return error
        }

		Scene* current_scene() {
			return nullptr;
		}

	private:
        int m_next_id = 1;
		ScenePtr m_active_scene;
		std::unordered_map<SceneID::value_type, std::function<ScenePtr()>> m_scene_constructors;
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

	scene_manager.load_scene(scene_id);
	EXPECT_EQ(TestScene::num_instances, 1);
}

// trying to load non-registered scene gives error

// switch between 2 scenes
