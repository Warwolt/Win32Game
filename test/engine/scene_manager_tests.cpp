#include <gtest/gtest.h>

namespace engine {

    struct Input;
    class Renderer;

	class Scene {
	public:
		virtual void update(const Input& input) = 0;
		virtual void draw(Renderer* renderer) = 0;
	};

	class SceneManager {
	public:
		// SceneID add_scene(???)
		// load scene
        Scene* current_scene() {
            return nullptr;
        }

	private:
		// active scene
		// registered scenes
	};

} // namespace engine

using namespace engine;

TEST(SceneManagerTests, InitiallyHoldsNoScene) {
	SceneManager scene_manager;

    Scene* current_scene = scene_manager.current_scene();

    EXPECT_EQ(current_scene, nullptr);
}
