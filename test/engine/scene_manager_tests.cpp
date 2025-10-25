#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/scene/scene_manager.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

using namespace engine;

struct TestScene : public Scene {
	static int num_instances;
	TestScene() { num_instances++; }
	~TestScene() { num_instances--; }
	void update(const Input&) {}
	void draw(Renderer*) {}
};

struct TestScene2 : public Scene {
	static int num_instances;
	TestScene2() { num_instances++; }
	~TestScene2() { num_instances--; }
	void update(const Input&) {}
	void draw(Renderer*) {}
};

int TestScene::num_instances = 0;
int TestScene2::num_instances = 0;

TEST(SceneManagerTests, InitiallyHoldsNoScene) {
	SceneManager scene_manager;

	Scene* current_scene = scene_manager.current_scene();

	EXPECT_EQ(current_scene, nullptr);
}

TEST_PARAMETERIZED(SceneManagerTests, LoadScene_LoadingWithBadID_GivesError, int, testing::Values(0, 1)) {
	SceneManager scene_manager;
	SceneID scene_id = SceneID(GetParam());

	std::expected<void, SceneManagerError> result = scene_manager.load_scene(scene_id);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), SceneManagerError::InvalidSceneId);
}

TEST(SceneManagerTests, LoadScene_RegisteredScene_SceneGetsLoaded) {
	SceneManager scene_manager;

	/* Register scene */
	SceneID scene_id = scene_manager.register_scene([]() {
		return std::make_unique<TestScene>();
	});
	EXPECT_EQ(TestScene::num_instances, 0);

	/* Load scene */
	std::expected<void, SceneManagerError> result = scene_manager.load_scene(scene_id);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_NE(scene_manager.current_scene(), nullptr);
}

TEST(SceneManagerTests, LoadScene_TwoRegisteredScenes_CanSwapBetweenThem) {
	SceneManager scene_manager;

	/* Register two scenes */
	SceneID scene_id = scene_manager.register_scene([]() {
		return std::make_unique<TestScene>();
	});
	SceneID scene_id2 = scene_manager.register_scene([]() {
		return std::make_unique<TestScene2>();
	});
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load first scene */
	std::expected<void, SceneManagerError> result = scene_manager.load_scene(scene_id);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load second scene */
	std::expected<void, SceneManagerError> result2 = scene_manager.load_scene(scene_id2);
	EXPECT_TRUE(result2.has_value());
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 1);
}
