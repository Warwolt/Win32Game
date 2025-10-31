#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/scene/scene_manager.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

using namespace engine;

struct TestScene : public Scene {
	static constexpr char NAME[] = "TestScene";
	static int num_instances;
	TestScene(ResourceManager*) { num_instances++; }
	~TestScene() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

struct TestScene2 : public Scene {
	static constexpr char NAME[] = "TestScene2";
	static int num_instances;
	TestScene2(ResourceManager*) { num_instances++; }
	~TestScene2() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

int TestScene::num_instances = 0;
int TestScene2::num_instances = 0;

TEST(SceneManagerTests, InitiallyHoldsNoScene) {
	SceneManager scene_manager;

	Scene* current_scene = scene_manager.current_scene();

	EXPECT_EQ(current_scene, nullptr);
}

TEST(SceneManagerTests, LoadScene_NonRegisteredScene_GivesError) {
	SceneManager scene_manager;
	ResourceManager resource_manager;

	std::expected<void, SceneManagerError> result = scene_manager.load_scene("bad scene name", &resource_manager);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), SceneManagerError::InvalidSceneName);
}

TEST(SceneManagerTests, LoadScene_RegisteredScene_SceneGetsLoaded) {
	SceneManager scene_manager;
	ResourceManager resource_manager;

	/* Register scene */
	scene_manager.register_scene<TestScene>();
	EXPECT_EQ(TestScene::num_instances, 0);

	/* Load scene */
	std::expected<void, SceneManagerError> result = scene_manager.load_scene(TestScene::NAME, &resource_manager);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_NE(scene_manager.current_scene(), nullptr);
}

TEST(SceneManagerTests, LoadScene_TwoRegisteredScenes_CanSwapBetweenThem) {
	SceneManager scene_manager;
	ResourceManager resource_manager;

	/* Register two scenes */
	scene_manager.register_scene<TestScene>();
	scene_manager.register_scene<TestScene2>();
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load first scene */
	std::expected<void, SceneManagerError> result = scene_manager.load_scene(TestScene::NAME, &resource_manager);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load second scene */
	std::expected<void, SceneManagerError> result2 = scene_manager.load_scene(TestScene2::NAME, &resource_manager);
	EXPECT_TRUE(result2.has_value());
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 1);
}
