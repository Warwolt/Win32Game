#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/input/input_bindings.h>
#include <engine/input/keyboard.h>

#include <string>

using namespace engine;

const std::string TEST_ACTION = "my_action";
constexpr int TEST_ACTION_KEY = 111;
constexpr int TEST_ACTION_KEY_2 = 222;

#define EXPECT_ACTION_PRESSED(bindings, action_name)            \
	EXPECT_TRUE(bindings.action_is_pressed(action_name));       \
	EXPECT_FALSE(bindings.action_was_pressed_now(action_name)); \
	EXPECT_FALSE(bindings.action_is_released(action_name));     \
	EXPECT_FALSE(bindings.action_was_released_now(action_name))

#define EXPECT_ACTION_PRESSED_NOW(bindings, action_name)       \
	EXPECT_TRUE(bindings.action_is_pressed(action_name));      \
	EXPECT_TRUE(bindings.action_was_pressed_now(action_name)); \
	EXPECT_FALSE(bindings.action_is_released(action_name));    \
	EXPECT_FALSE(bindings.action_was_released_now(action_name))

#define EXPECT_ACTION_RELEASED(bindings, action_name)           \
	EXPECT_FALSE(bindings.action_is_pressed(action_name));      \
	EXPECT_FALSE(bindings.action_was_pressed_now(action_name)); \
	EXPECT_TRUE(bindings.action_is_released(action_name));      \
	EXPECT_FALSE(bindings.action_was_released_now(action_name))

#define EXPECT_ACTION_RELEASED_NOW(bindings, action_name)       \
	EXPECT_FALSE(bindings.action_is_pressed(action_name));      \
	EXPECT_FALSE(bindings.action_was_pressed_now(action_name)); \
	EXPECT_TRUE(bindings.action_is_released(action_name));      \
	EXPECT_TRUE(bindings.action_was_released_now(action_name))

TEST(InputBindingsTests, InitiallyReleased) {
	InputBindings bindings;
	EXPECT_ACTION_RELEASED(bindings, TEST_ACTION);
}

TEST_PARAMETERIZED(InputBindingsTests, TwoActionKeys_EitherIsPressed_ActionIsPressed, int, testing::Values(TEST_ACTION_KEY, TEST_ACTION_KEY_2)) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY, TEST_ACTION_KEY_2 });
	const int either_key = GetParam();

	keyboard.on_key_event(either_key, true);
	keyboard.update();
	bindings.update(keyboard);

	EXPECT_ACTION_PRESSED_NOW(bindings, TEST_ACTION);
}

TEST_PARAMETERIZED(InputBindingsTests, TwoActionKeys_EitherIsReleased_ActionIsReleased, int, testing::Values(TEST_ACTION_KEY, TEST_ACTION_KEY_2)) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY, TEST_ACTION_KEY_2 });
	const int either_key = GetParam();

	/* Press */
	keyboard.on_key_event(either_key, true);
	keyboard.update();
	bindings.update(keyboard);

	/* Release */
	keyboard.on_key_event(either_key, false);
	keyboard.update();
	bindings.update(keyboard);

	EXPECT_ACTION_RELEASED_NOW(bindings, TEST_ACTION);
}

TEST(InputBindingsTests, PressKeysInSequence_ActionIsPressed) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY, TEST_ACTION_KEY_2 });

	/* Press first key */
	keyboard.on_key_event(TEST_ACTION_KEY, true);
	keyboard.update();
	bindings.update(keyboard);
	EXPECT_ACTION_PRESSED_NOW(bindings, TEST_ACTION);

	/* Press second key */
	keyboard.on_key_event(TEST_ACTION_KEY_2, true);
	keyboard.update();
	bindings.update(keyboard);
	EXPECT_ACTION_PRESSED(bindings, TEST_ACTION);
}

TEST(InputBindingsTests, TwoKeysPressed_ActionIsReleased_WhenLastKeyReleased) {
	InputBindings bindings;
	Keyboard keyboard;
	bindings.add_keyboard_binding(TEST_ACTION, { TEST_ACTION_KEY, TEST_ACTION_KEY_2 });

	/* Press first key */
	keyboard.on_key_event(TEST_ACTION_KEY, true);
	keyboard.update();
	bindings.update(keyboard);

	/* Press second key */
	keyboard.on_key_event(TEST_ACTION_KEY_2, true);
	keyboard.update();
	bindings.update(keyboard);

	/* Release first key */
	keyboard.on_key_event(TEST_ACTION_KEY, false);
	keyboard.update();
	bindings.update(keyboard);
	EXPECT_ACTION_PRESSED(bindings, TEST_ACTION); // <-- should still be pressed!

	/* Release second key */
	keyboard.on_key_event(TEST_ACTION_KEY_2, false);
	keyboard.update();
	bindings.update(keyboard);
	EXPECT_ACTION_RELEASED_NOW(bindings, TEST_ACTION);
}
