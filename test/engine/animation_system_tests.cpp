#include <gtest/gtest.h>

#include <engine/animation/animation_system.h>

using namespace engine;

TEST(AnimationSystemTests, Hello) {
	AnimationSystem<int> animation_system;
	EXPECT_EQ(2 + 2, 5) << "Hello!";
}
