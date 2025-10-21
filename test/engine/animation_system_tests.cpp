#include <gtest/gtest.h>

#include <test_helpers/parameterized_tests.h>

#include <engine/animation/animation_system.h>

using namespace engine;

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

class AnimationSystemTests : public testing::Test {};

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

	std::expected<void, AnimationError> result = animation_system.start_animation(INVALID_ANIMATION_ID, entity_id, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationID);
}

TEST(AnimationSystemTests, StartAnimation_NonRegisteredAnimationID_GivesError) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = AnimationID(1); // we haven't added this animation to the system
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, entity_id, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationID);
}

TEST(AnimationSystemTests, StartAnimation_InvalidAnimationEntityID_GivesError) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, INVALID_ANIMATION_ENTITY_ID, 0ms);

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), AnimationError::InvalidAnimationEntityID);
}

struct AnimationPlaybackTestData {
	Time time_now;
	int expected_frame;
	const char* name;
};
AnimationPlaybackTestData test_data[] = {
	// clang-format off
	{ -1ms,  FRAME_ONE,   "negative_1ms_GivesFirstFrame" },
	{ 0ms,   FRAME_ONE,   "0ms_GivesFirstFrame" },
	{ 50ms,  FRAME_ONE,   "50ms_GivesFirstFrame" },
	{ 100ms, FRAME_TWO,   "100ms_GivesSecondFrame" },
	{ 150ms, FRAME_TWO,   "150ms_GivesSecondFrame" },
	{ 200ms, FRAME_THREE, "200ms_GivesThirdFrame" },
	{ 300ms, FRAME_THREE, "300ms_GivesThirdFrame" },
	// clang-format on
};
PARAMETERIZED_TEST(AnimationSystemTests, AnimationPlayback, AnimationPlaybackTestData, testing::ValuesIn(test_data)) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, entity_id, 0ms);
	animation_system.update(GetParam().time_now);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, GetParam().expected_frame);
}
