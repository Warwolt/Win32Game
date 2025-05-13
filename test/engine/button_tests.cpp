#include <gtest/gtest.h>

#include <engine/input/button.h>

using namespace engine;

TEST(ButtonTests, InitiallyReleased) {
	Button button;
	EXPECT_TRUE(button.is_released());
    EXPECT_FALSE(button.is_just_released());
	EXPECT_FALSE(button.is_pressed());
	EXPECT_FALSE(button.is_just_pressed());
}

TEST(ButtonTests, Pressed) {
	Button button;

    button.update(true);

	EXPECT_FALSE(button.is_released());
    EXPECT_FALSE(button.is_just_released());
	EXPECT_TRUE(button.is_pressed());
	EXPECT_TRUE(button.is_just_pressed());
}

TEST(ButtonTests, HeldDown) {
	Button button;

    button.update(true);
    button.update(true);

	EXPECT_FALSE(button.is_released());
    EXPECT_FALSE(button.is_just_released());
	EXPECT_TRUE(button.is_pressed());
	EXPECT_FALSE(button.is_just_pressed());
}

TEST(ButtonTests, Released) {
	Button button;

    button.update(true);
    button.update(false);

	EXPECT_TRUE(button.is_released());
	EXPECT_TRUE(button.is_just_released());
	EXPECT_FALSE(button.is_pressed());
	EXPECT_FALSE(button.is_just_pressed());
}


TEST(ButtonTests, HeldUp) {
	Button button;

    button.update(true);
    button.update(false);
    button.update(false);

	EXPECT_TRUE(button.is_released());
	EXPECT_FALSE(button.is_just_released());
	EXPECT_FALSE(button.is_pressed());
	EXPECT_FALSE(button.is_just_pressed());
}
