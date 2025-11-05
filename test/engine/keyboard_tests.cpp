#include <gtest/gtest.h>

#include <engine/input/keyboard.h>

using namespace engine;

constexpr uint32_t TEST_KEY_ID = 1234;

TEST(KeyboardTests, InitiallyReleased) {
	Keyboard keyboard;

    EXPECT_TRUE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_pressed(TEST_KEY_ID));
    EXPECT_FALSE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}

TEST(KeyboardTests, Pressed) {
	Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

	EXPECT_FALSE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_is_pressed(TEST_KEY_ID));
    EXPECT_FALSE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}

TEST(KeyboardTests, Pressed_WhileIgnoringEvents) {
	Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true); // should be ignored
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

	EXPECT_FALSE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_is_pressed(TEST_KEY_ID));
    EXPECT_FALSE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}

TEST(KeyboardTests, HeldDown) {
	Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

    // keep held down
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

	EXPECT_FALSE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_is_pressed(TEST_KEY_ID));
    EXPECT_FALSE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}

TEST(KeyboardTests, Released) {
	Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

    // release
    keyboard.on_key_event(TEST_KEY_ID, false);
    keyboard.update();

	EXPECT_TRUE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_pressed(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}


TEST(KeyboardTests, HeldUp) {
	Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.update();

    // release
    keyboard.on_key_event(TEST_KEY_ID, false);
    keyboard.update();

    // hold up
    keyboard.on_key_event(TEST_KEY_ID, false);
    keyboard.update();

	EXPECT_TRUE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_pressed(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}

// From bug where up event not registered after holding down key for long time
TEST(KeyboardTests, RepeatedDownEvents_UpEvent) {
    Keyboard keyboard;

    // push
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.on_key_event(TEST_KEY_ID, true);
    keyboard.on_key_event(TEST_KEY_ID, true);

    keyboard.update();

    // release
    keyboard.on_key_event(TEST_KEY_ID, false);

    keyboard.update();

    EXPECT_TRUE(keyboard.key_is_released(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_is_pressed(TEST_KEY_ID));
	EXPECT_TRUE(keyboard.key_was_released_now(TEST_KEY_ID));
	EXPECT_FALSE(keyboard.key_was_pressed_now(TEST_KEY_ID));
}
