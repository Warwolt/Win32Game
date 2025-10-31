#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/ui/screen_stack.h>

#include <expected>
#include <functional>
#include <memory>
#include <unordered_map>

using namespace engine;

struct TestScreen : public Screen {
	static int num_instances;
	TestScreen() { num_instances++; }
	~TestScreen() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

struct TestScreen2 : public Screen {
	static int num_instances;
	TestScreen2() { num_instances++; }
	~TestScreen2() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

int TestScreen::num_instances = 0;
int TestScreen2::num_instances = 0;

TEST(ScreenStackTests, InitiallyHoldsNoScreen) {
	ScreenStack screen_stack;

	Screen* current_screen = screen_stack.current_screen();

	EXPECT_EQ(current_screen, nullptr);
}

TEST(ScreenStackTests, ShowScreen_NonRegisteredScreen_GivesError) {
	ScreenStack screen_stack;
	ResourceManager resource_manager;

	std::expected<void, ScreenStackError> result = screen_stack.show_screen("bad screen name", &resource_manager);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), ScreenStackError::InvalidSceeenName);
}

TEST(ScreenStackTests, ShowScreen_RegisteredScreen_ScreenGetsShown) {
	ScreenStack screen_stack;
	ResourceManager resource_manager;

	/* Register screen */
	screen_stack.register_screen("my screen", [](ResourceManager*) {
		return std::make_unique<TestScreen>();
	});
	EXPECT_EQ(TestScreen::num_instances, 0);

	/* Load screen */
	std::expected<void, ScreenStackError> result = screen_stack.show_screen("my screen", &resource_manager);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScreen::num_instances, 1);
	EXPECT_NE(screen_stack.current_screen(), nullptr);
}

TEST(ScreenStackTests, ShowScreen_ScreenAlreadyShown_DoesNothing) {
	ScreenStack screen_stack;
	ResourceManager resource_manager;

	/* Register screen */
	screen_stack.register_screen("my screen", [](ResourceManager*) {
		return std::make_unique<TestScreen>();
	});
	EXPECT_EQ(TestScreen::num_instances, 0);

	/* Load screen */
	std::expected<void, ScreenStackError> result = screen_stack.show_screen("my screen", &resource_manager);
    std::expected<void, ScreenStackError> result2 = screen_stack.show_screen("my screen", &resource_manager);
	EXPECT_TRUE(result.has_value());
	EXPECT_TRUE(result2.has_value());
	EXPECT_EQ(TestScreen::num_instances, 1);
	EXPECT_NE(screen_stack.current_screen(), nullptr);
}

// TEST(SceneManagerTests, LoadScene_TwoRegisteredScenes_CanSwapBetweenThem) {
// 	SceneManager scene_manager;
// 	ResourceManager resource_manager;

// 	/* Register two scenes */
// 	scene_manager.register_scene("first scene", [](ResourceManager*) {
// 		return std::make_unique<TestScene>();
// 	});
// 	scene_manager.register_scene("second scene", [](ResourceManager*) {
// 		return std::make_unique<TestScene2>();
// 	});
// 	EXPECT_EQ(TestScene::num_instances, 0);
// 	EXPECT_EQ(TestScene2::num_instances, 0);

// 	/* Load first scene */
// 	std::expected<void, SceneManagerError> result = scene_manager.load_scene("first scene", &resource_manager);
// 	EXPECT_TRUE(result.has_value());
// 	EXPECT_EQ(TestScene::num_instances, 1);
// 	EXPECT_EQ(TestScene2::num_instances, 0);

// 	/* Load second scene */
// 	std::expected<void, SceneManagerError> result2 = scene_manager.load_scene("second scene", &resource_manager);
// 	EXPECT_TRUE(result2.has_value());
// 	EXPECT_EQ(TestScene::num_instances, 0);
// 	EXPECT_EQ(TestScene2::num_instances, 1);
// }
