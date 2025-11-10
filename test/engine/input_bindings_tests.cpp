#include <gtest/gtest.h>

#include <engine/input/input_bindings.h>
#include <engine/input/keyboard.h>

#include <string>

using namespace engine;

const std::string TEST_ACTION = "my_action";
constexpr int TEST_ACTION_KEY = 123;

TEST(InputBindingsTests, InitiallyReleased) {
	InputBindings bindings;

	EXPECT_TRUE(bindings.action_is_released(TEST_ACTION));
	EXPECT_FALSE(bindings.action_is_pressed(TEST_ACTION));
	EXPECT_FALSE(bindings.action_was_released_now(TEST_ACTION));
	EXPECT_FALSE(bindings.action_was_pressed_now(TEST_ACTION));
}

TEST(InputBindingsTest, ActionKeyPressed_ActionIsPressed) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY });

    keyboard.on_key_event(TEST_ACTION_KEY, true);
    keyboard.update();
    bindings.update(keyboard);

    EXPECT_FALSE(bindings.action_is_released(TEST_ACTION));
    EXPECT_TRUE(bindings.action_is_pressed(TEST_ACTION));
    EXPECT_FALSE(bindings.action_was_released_now(TEST_ACTION));
    EXPECT_TRUE(bindings.action_was_pressed_now(TEST_ACTION));
}

TEST(InputBindingsTest, ActionKeyReleased_ActionIsReleased) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY });

    keyboard.on_key_event(TEST_ACTION_KEY, true);
    keyboard.update();
    bindings.update(keyboard);

    EXPECT_FALSE(bindings.action_is_released(TEST_ACTION));
    EXPECT_TRUE(bindings.action_is_pressed(TEST_ACTION));
    EXPECT_FALSE(bindings.action_was_released_now(TEST_ACTION));
    EXPECT_TRUE(bindings.action_was_pressed_now(TEST_ACTION));
}

// either of two buttons pressed -> action pressed

// either of two buttons released -> action pressed

// press first button -> pressed now ; press second button -> is pressed

// release first button -> released now ; release second button -> is released
