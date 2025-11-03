#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/animation/animation.h>

using namespace engine;
using namespace std::chrono_literals;

constexpr int FRAME_ONE = 111;
constexpr int FRAME_TWO = 222;
constexpr int FRAME_THREE = 333;

const std::vector<AnimationFrame<int>> TEST_ANIMATION = {
	{ .value = FRAME_ONE, .duration = 100ms },
	{ .value = FRAME_TWO, .duration = 100ms },
	{ .value = FRAME_THREE, .duration = 100ms },

};

TEST(AnimationPlayerTests, DefaultConstructed_HasDefaultValue) {
	AnimationPlayer<int> player;

	const Time time_now = 0ms;
	const int& value = player.value();

	EXPECT_EQ(value, int {});
}

TEST(AnimationPlayerTests, PlayAnimation_MissingInLibrary_GivesError) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;

	const Time time_now = 0ms;
	std::optional<AnimationError> error = player.play(library, AnimationID(1), time_now);
	ASSERT_TRUE(error.has_value());
	EXPECT_EQ(error.value(), AnimationError::MissingAnimationInLibrary);
}

TEST(AnimationPlayerTests, PlayAnimation_StartsAtFirstFrame) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	const Time time_now = 0ms;
	std::optional<AnimationError> error = player.play(library, AnimationID(1), time_now);
	EXPECT_FALSE(error.has_value());

	const int value = player.value();
	EXPECT_EQ(value, FRAME_ONE);
}
