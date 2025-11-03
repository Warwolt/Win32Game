#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>

#include <engine/animation/animation.h>

using namespace engine;

using namespace std::chrono_literals;

TEST(AnimationPlayerTests, InitiallyHasaNoValue) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;

	const Time time_now = 0ms;
	const int* value = player.value(library);

	EXPECT_EQ(value, nullptr);
}

TEST(AnimationPlayerTests, PlayAnimation_MissingInLibrary_GivesError) {
	AnimationLibrary<int> library;
	AnimationPlayer<int> player;

	const Time time_now = 0ms;
	std::optional<AnimationError> error = player.play(library, AnimationID(1), time_now);
	ASSERT_TRUE(error.has_value());
	EXPECT_EQ(error.value(), AnimationError::MissingAnimationInLibrary);

	const int* value = player.value(library);
	EXPECT_EQ(value, nullptr);
}
