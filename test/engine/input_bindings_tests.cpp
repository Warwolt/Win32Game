#include <gtest/gtest.h>

#include <engine/input/input_bindings.h>
#include <engine/input/keyboard.h>

#include <string>

using namespace engine;

const std::string TEST_ACTION_ID = "my_action";

TEST(InputBindingsTests, InitiallyReleased) {
    InputBindings bindings;

    EXPECT_TRUE(bindings.action_is_released(TEST_ACTION_ID));
	EXPECT_FALSE(bindings.action_is_pressed(TEST_ACTION_ID));
    EXPECT_FALSE(bindings.action_was_released_now(TEST_ACTION_ID));
	EXPECT_FALSE(bindings.action_was_pressed_now(TEST_ACTION_ID));
}
