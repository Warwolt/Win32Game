#include <gtest/gtest.h>

#include <engine/ui/screen_stack.h>

using namespace engine;

struct TestScreen_A : public Screen {
	static constexpr char NAME[] = "TestScreen A";
	static int num_instances;
	TestScreen_A() { num_instances++; }
	~TestScreen_A() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

struct TestScreen_B : public Screen {
	static constexpr char NAME[] = "TestScreen B";
	static int num_instances;
	TestScreen_B() { num_instances++; }
	~TestScreen_B() { num_instances--; }
	void update(const Input&, CommandList*) override {}
	void draw(Renderer*) const override {}
};

int TestScreen_A::num_instances = 0;
int TestScreen_B::num_instances = 0;

TEST(ScreenStackTests, InitiallyHoldsNoScreen) {
	ScreenStack screen_stack;

	Screen* current_screen = screen_stack.current_screen();

	EXPECT_EQ(current_screen, nullptr);
}

TEST(ScreenStackTests, PushScreen_NonRegisteredScreen_GivesError) {
	ScreenStack screen_stack;

	std::expected<void, ScreenStackError> result = screen_stack.push_screen("bad screen name");

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), ScreenStackError::InvalidSceeenName);
}

TEST(ScreenStackTests, PushScreen_RegisteredScreen_ScreenGetsShown) {
	ScreenStack screen_stack;

	/* Register screen */
	screen_stack.register_screen<TestScreen_A>();
	EXPECT_EQ(TestScreen_A::num_instances, 0);

	/* Push screen */
	std::expected<void, ScreenStackError> result = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);
	EXPECT_NE(screen_stack.current_screen(), nullptr);
}

TEST(ScreenStackTests, PushScreen_SameScreenAlreadyShown_DoesNothing) {
	ScreenStack screen_stack;

	/* Register screen */
	screen_stack.register_screen<TestScreen_A>();
	EXPECT_EQ(TestScreen_A::num_instances, 0);

	/* Push screen */
	std::expected<void, ScreenStackError> result = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);

	/* Try to show same screen again */
	std::expected<void, ScreenStackError> result2 = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result2.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);
}

TEST(SceneManagerTests, PushScreen_InterleaveScreens) {
	ScreenStack screen_stack;

	/* Register two screens */
	screen_stack.register_screen<TestScreen_A>();
	screen_stack.register_screen<TestScreen_B>();
	EXPECT_EQ(TestScreen_A::num_instances, 0);
	EXPECT_EQ(TestScreen_B::num_instances, 0);

	/* Push screen A */
	std::expected<void, ScreenStackError> result = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);
	EXPECT_EQ(TestScreen_B::num_instances, 0);

	/* Push screen B */
	std::expected<void, ScreenStackError> result2 = screen_stack.push_screen(TestScreen_B::NAME);
	EXPECT_TRUE(result2.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);
	EXPECT_EQ(TestScreen_B::num_instances, 1);

	/* Push screen A again */
	std::expected<void, ScreenStackError> result3 = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result3.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 2);
	EXPECT_EQ(TestScreen_B::num_instances, 1);
}

TEST(ScreenManagerTests, PushScreen_PopScreen) {
	ScreenStack screen_stack;

	/* Register screen */
	screen_stack.register_screen<TestScreen_A>();
	EXPECT_EQ(TestScreen_A::num_instances, 0);

	/* Push screen */
	std::expected<void, ScreenStackError> result = screen_stack.push_screen(TestScreen_A::NAME);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(TestScreen_A::num_instances, 1);

	/* Pop screen */
	screen_stack.pop_screen();
	EXPECT_EQ(TestScreen_A::num_instances, 0);
}
