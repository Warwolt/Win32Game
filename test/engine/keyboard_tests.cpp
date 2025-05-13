#include <gtest/gtest.h>

#include <engine/input/keyboard.h>

using namespace engine;

constexpr uint32_t TEST_KEY_ID = 1234;

TEST(KeyboardTests, KeysAreInitiallyReleased) {
	Keyboard keyboard;

    EXPECT_TRUE(keyboard.key_is_released(TEST_KEY_ID));
    EXPECT_FALSE(keyboard.key_is_just_released(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_pressed(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_just_pressed(TEST_KEY_ID));
}
