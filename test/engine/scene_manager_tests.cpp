#include <gtest/gtest.h>

#include <engine/scene/scene_manager.h>

#include <memory>

using namespace engine;

struct TestScene : public Scene {
	static constexpr char NAME[] = "TestScene";
	static int num_instances;
	TestScene() { num_instances++; }
	~TestScene() { num_instances--; }
	void update(game::GameData*, const Input&, CommandList*) override {}
	void draw(const game::GameData&, Renderer*) const override {}
};

struct TestScene2 : public Scene {
	static constexpr char NAME[] = "TestScene2";
	static int num_instances;
	TestScene2() { num_instances++; }
	~TestScene2() { num_instances--; }
	void update(game::GameData*, const Input&, CommandList*) override {}
	void draw(const game::GameData&, Renderer*) const override {}
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

	std::optional<SceneManagerError> error = scene_manager.load_scene("bad scene name");

	ASSERT_TRUE(error.has_value());
	EXPECT_EQ(error.value(), SceneManagerError::InvalidSceneName);
}

TEST(SceneManagerTests, LoadScene_RegisteredScene_SceneGetsLoaded) {
	SceneManager scene_manager;

	/* Register scene */
	scene_manager.register_scene(TestScene::NAME, []() { return std::make_unique<TestScene>(); });
	EXPECT_EQ(TestScene::num_instances, 0);

	/* Load scene */
	std::optional<SceneManagerError> error = scene_manager.load_scene(TestScene::NAME);
	EXPECT_FALSE(error.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_NE(scene_manager.current_scene(), nullptr);
}

TEST(SceneManagerTests, LoadScene_TwoRegisteredScenes_CanSwapBetweenThem) {
	SceneManager scene_manager;

	/* Register two scenes */
	scene_manager.register_scene(TestScene::NAME, []() { return std::make_unique<TestScene>(); });
	scene_manager.register_scene(TestScene2::NAME, []() { return std::make_unique<TestScene2>(); });
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load first scene */
	std::optional<SceneManagerError> error = scene_manager.load_scene(TestScene::NAME);
	EXPECT_FALSE(error.has_value());
	EXPECT_EQ(TestScene::num_instances, 1);
	EXPECT_EQ(TestScene2::num_instances, 0);

	/* Load second scene */
	std::optional<SceneManagerError> error2 = scene_manager.load_scene(TestScene2::NAME);
	EXPECT_FALSE(error.has_value());
	EXPECT_EQ(TestScene::num_instances, 0);
	EXPECT_EQ(TestScene2::num_instances, 1);
}
