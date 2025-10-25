#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/animation/animation_system.h>

using namespace engine;

struct AnimationPlaybackTestData {
	Time time_now;
	int expected_frame;
	const char* name;
};

constexpr int FRAME_ONE = 111;
constexpr int FRAME_TWO = 222;
constexpr int FRAME_THREE = 333;
std::vector<AnimationFrame<int>> test_animation() {
	return {
		{ FRAME_ONE, 100ms },
		{ FRAME_TWO, 100ms },
		{ FRAME_THREE, 100ms },
	};
}

TEST(AnimationSystemTests, AddAnimation_NoFrames_GivesInvalidAnimationID) {
	AnimationSystem<int> animation_system;

	AnimationID animation_id = animation_system.add_animation({});

	EXPECT_EQ(animation_id, INVALID_ANIMATION_ID);
}

TEST(AnimationSystemTests, CurrentFrame_InvalidAnimationID_GivesDefaultAnimationFrame) {
	AnimationSystem<int> animation_system;

	int current_frame = animation_system.current_frame(INVALID_ANIMATION_ENTITY_ID);

	EXPECT_EQ(current_frame, 0);
}

TEST(AnimationSystemTests, StartAnimation_InvalidAnimationID_GivesError) {
	AnimationSystem<int> animation_system;
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(entity_id, INVALID_ANIMATION_ID, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationID);
}

TEST(AnimationSystemTests, StartAnimation_NonRegisteredAnimationID_GivesError) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = AnimationID(1); // we haven't added this animation to the system
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(entity_id, animation_id, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationID);
}

TEST(AnimationSystemTests, StartAnimation_InvalidAnimationEntityID_GivesError) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());

	std::expected<void, AnimationError> result = animation_system.start_animation(INVALID_ANIMATION_ENTITY_ID, animation_id, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationEntityID);
}

TEST(AnimationSystemTests, StartAnimation_AlreadyStarted_DoesNothing) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> first_result = animation_system.start_animation(entity_id, animation_id, 0ms);
	std::expected<void, AnimationError> second_result = animation_system.start_animation(entity_id, animation_id, 50ms);
	animation_system.update(100ms);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(first_result.has_value());
	EXPECT_TRUE(second_result.has_value());
	EXPECT_EQ(current_frame, FRAME_TWO);
}

TEST(AnimationSystemTests, StopAnimation_StaysAtLastFrame) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(entity_id, animation_id, 0ms);
	animation_system.update(100ms);
	animation_system.stop_animation(entity_id);
	animation_system.update(200ms);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, FRAME_TWO);
}

TEST(AnimationSystemTests, RestartAnimation_GoesBackToFirstFrame) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> start_result = animation_system.start_animation(entity_id, animation_id, 0ms);
	animation_system.update(100ms);
	animation_system.stop_animation(entity_id);
	int stopped_frame = animation_system.current_frame(entity_id);
	std::expected<void, AnimationError> restart_result = animation_system.restart_animation(entity_id, animation_id, 100ms);
	int restarted_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(start_result.has_value());
	EXPECT_TRUE(restart_result.has_value());
	EXPECT_EQ(stopped_frame, FRAME_TWO);
	EXPECT_EQ(restarted_frame, FRAME_ONE);
}

AnimationPlaybackTestData animation_playback_tests[] = {
	{ -1ms, FRAME_ONE, "negative_1ms_GivesFirstFrame" },
	{ 0ms, FRAME_ONE, "0ms_GivesFirstFrame" },
	{ 50ms, FRAME_ONE, "50ms_GivesFirstFrame" },
	{ 100ms, FRAME_TWO, "100ms_GivesSecondFrame" },
	{ 150ms, FRAME_TWO, "150ms_GivesSecondFrame" },
	{ 200ms, FRAME_THREE, "200ms_GivesThirdFrame" },
	{ 300ms, FRAME_THREE, "300ms_GivesThirdFrame" },
};
PARAMETERIZED_TEST(AnimationSystemTests, AnimationPlayback, AnimationPlaybackTestData, testing::ValuesIn(animation_playback_tests)) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(entity_id, animation_id, 0ms);
	animation_system.update(GetParam().time_now);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, GetParam().expected_frame);
}

AnimationPlaybackTestData looping_animation_playback_tests[] = {
	{ 0ms, FRAME_ONE, "0ms_GivesFirstFrame" },
	{ 100ms, FRAME_TWO, "100ms_GivesSecondFrame" },
	{ 200ms, FRAME_THREE, "200ms_GivesThirdFrame" },
	{ 300ms, FRAME_ONE, "300ms_LoopsAndGivesFirstFrame" },
	{ 400ms, FRAME_TWO, "400ms_LoopsAndGivesSecondFrame" },
	{ 500ms, FRAME_THREE, "500ms_LoopsAndGivesThirdFrame" },
};
PARAMETERIZED_TEST(AnimationSystemTests, LoopedAnimationPlayback, AnimationPlaybackTestData, testing::ValuesIn(looping_animation_playback_tests)) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation(), { .looping = true });
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(entity_id, animation_id, 0ms);
	animation_system.update(GetParam().time_now);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, GetParam().expected_frame);
}
