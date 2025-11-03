#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/animation/animation.h>

using namespace engine;
using namespace std::chrono_literals;

constexpr int FRAME_ONE_VALUE = 111;
constexpr int FRAME_TWO_VALUE = 222;
constexpr int FRAME_THREE_VALUE = 333;

const Time FRAME_ONE_DURATION = 100ms;
const Time FRAME_TWO_DURATION = 100ms;
const Time FRAME_THREE_DURATION = 100ms;

const std::vector<AnimationFrame<int>> TEST_ANIMATION = {
	{ .value = FRAME_ONE_VALUE, .duration = FRAME_ONE_DURATION },
	{ .value = FRAME_TWO_VALUE, .duration = FRAME_TWO_DURATION },
	{ .value = FRAME_THREE_VALUE, .duration = FRAME_THREE_DURATION },
};

TEST(AnimationPlayerTests, DefaultConstructed_HasDefaultValue) {
	AnimationPlayer<int> player;

	const int& value = player.value();

	EXPECT_EQ(value, int {});
}

TEST(AnimationPlayerTests, PlayAnimation_MissingInLibrary_GivesError) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;

	std::optional<AnimationError> error = player.play(library, AnimationID(1), 0ms);
	ASSERT_TRUE(error.has_value());
	EXPECT_EQ(error.value(), AnimationError::MissingAnimationInLibrary);
}

TEST(AnimationPlayerTests, PlayAnimation_StartsAtFirstFrame) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	std::optional<AnimationError> error = player.play(library, animation_id, 0ms);
	EXPECT_FALSE(error.has_value());

	const int value = player.value();
	EXPECT_EQ(value, FRAME_ONE_VALUE);
}

TEST(AnimationPlayerTests, PlayAnimation_NonLoopingAnimation) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	/* Play animation */
	const Time start_time = 0ms;
	std::optional<AnimationError> error = player.play(library, animation_id, start_time);
	EXPECT_FALSE(error.has_value());

	/* First frame */
	player.update(library, start_time + 0ms);
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);

	/* Second frame */
	player.update(library, start_time + FRAME_ONE_DURATION);
	EXPECT_EQ(player.value(), FRAME_TWO_VALUE);

	/* Third frame */
	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION);
	EXPECT_EQ(player.value(), FRAME_THREE_VALUE);

	/* Past last frame */
	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION + FRAME_THREE_DURATION);
	EXPECT_EQ(player.value(), FRAME_THREE_VALUE);
}

TEST(AnimationPlayerTests, PlayAnimation_LoopingAnimation) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	/* Play animation */
	const Time start_time = 0ms;
	std::optional<AnimationError> error = player.play(library, animation_id, start_time, { .looping = true });
	EXPECT_FALSE(error.has_value());

	/* First frame */
	player.update(library, start_time + 0ms);
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);

	/* Second frame */
	player.update(library, start_time + FRAME_ONE_DURATION);
	EXPECT_EQ(player.value(), FRAME_TWO_VALUE);

	/* Third frame */
	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION);
	EXPECT_EQ(player.value(), FRAME_THREE_VALUE);

	/* First frame again */
	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION + FRAME_THREE_DURATION);
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);

	/* Second frame again */
	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION + FRAME_THREE_DURATION + FRAME_ONE_DURATION);
	EXPECT_EQ(player.value(), FRAME_TWO_VALUE);
}

TEST(AnimationPlayerTests, PlayAnimation_AlreadyPlaying_Resetarts) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	/* Play animation */
	const Time start_time = 0ms;
	std::optional<AnimationError> error = player.play(library, animation_id, start_time);
	EXPECT_FALSE(error.has_value());
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);

	/* Update into second frame */
	player.update(library, start_time + FRAME_ONE_DURATION);
	EXPECT_EQ(player.value(), FRAME_TWO_VALUE);

	/* Restart same animation */
	std::optional<AnimationError> error2 = player.play(library, animation_id, start_time + 50ms);
	EXPECT_FALSE(error2.has_value());
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);
}

TEST(AnimationPlayerTests, PauseAnimation_HoldsSameFrame) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;
	AnimationID animation_id = library.add_animation(TEST_ANIMATION);

	/* Start animation */
	const Time start_time = 0ms;
	std::optional<AnimationError> error = player.play(library, animation_id, start_time);
	EXPECT_FALSE(error.has_value());

	/* Pause animation */
	player.pause();

	/* Holds same frame */
	player.update(library, start_time + FRAME_ONE_DURATION);
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);

	player.update(library, start_time + FRAME_ONE_DURATION + FRAME_TWO_DURATION);
	EXPECT_EQ(player.value(), FRAME_ONE_VALUE);
}

// set animation frame, jumps there immediately
