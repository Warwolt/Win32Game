#include <gtest/gtest.h>

#include <engine/animation/animation_system.h>

using namespace engine;

std::vector<AnimationFrame<int>> test_animation() {
	return {
		{ 111, 100ms },
		{ 222, 100ms },
		{ 333, 100ms },
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

TEST(AnimationSystemTests, AnimationPlayback_Wait0ms_PlayingFirstFrame) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, entity_id, 0ms);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, 111);
}

TEST(AnimationSystemTests, AnimationPlayback_Wait50ms_PlayingFirstFrame) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, entity_id, 50ms);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, 111);
}

TEST(AnimationSystemTests, AnimationPlayback_Wait100ms_PlayingSecondFrame) {
	AnimationSystem<int> animation_system;
	AnimationID animation_id = animation_system.add_animation(test_animation());
	AnimationEntityID entity_id = AnimationEntityID(1);

	std::expected<void, AnimationError> result = animation_system.start_animation(animation_id, entity_id, 0ms);
	animation_system.update(100ms);
	int current_frame = animation_system.current_frame(entity_id);

	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(current_frame, 222);
}

struct PlaybackTestCase {
	int x;
	const char* name;
};

std::vector<PlaybackTestCase> my_test_data = {
	{ 1, "0ms_GivesFirstFrame" },
	{ 2, "50ms_GivesFirstFrame" },
	{ 3, "100ms_GivesSecondFrame" }
};

class AnimationPlayback : public AnimationSystemTests
	, public testing::WithParamInterface<PlaybackTestCase> {
};
#pragma warning(push)
#pragma warning(disable: 4003)
INSTANTIATE_TEST_SUITE_P(AnimationSystemTests, AnimationPlayback, testing::ValuesIn(my_test_data), [](testing::TestParamInfo<AnimationPlayback::ParamType> param) {
	return param.param.name;
});
TEST_P(AnimationPlayback)
#pragma warning(pop)
{
	printf("%d\n", GetParam().x);
}
