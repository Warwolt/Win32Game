#include <gtest/gtest.h>

#include <engine/container/inplace_vector.h>

TEST(InplaceVectorTests, DefaultVector_IsEmpty) {
	engine::inplace_vector<int, 3> vec;
	EXPECT_TRUE(vec.empty());
	EXPECT_EQ(vec.size(), 0);
}

TEST(InplaceVectorTests, PushedElements_CanBeRetreived) {
	engine::inplace_vector<int, 3> vec;

	vec.push_back(10);
	vec.push_back(11);
	vec.push_back(12);

	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 11);
	EXPECT_EQ(vec[2], 12);
	EXPECT_EQ(vec.size(), 3);
}

TEST(InplaceVectorTests, PushedElements_BeyondCapacity_AreIgnored) {
	engine::inplace_vector<int, 3> vec;

	vec.push_back(10);
	vec.push_back(11);
	vec.push_back(12);
	vec.push_back(13);
	vec.push_back(14);

	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 11);
	EXPECT_EQ(vec[2], 12);
	EXPECT_EQ(vec.size(), 3);
}

TEST(InplaceVectorTests, InitializerListConstructor_CopiesElements) {
	engine::inplace_vector<int, 3> vec = { 1, 2, 3 };

	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[1], 2);
	EXPECT_EQ(vec[2], 3);
	EXPECT_EQ(vec.size(), 3);
}

TEST(InplaceVectorTests, InitializerListConstructor_DiscardsBeyondCapacity) {
	engine::inplace_vector<int, 3> vec = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[1], 2);
	EXPECT_EQ(vec[2], 3);
	EXPECT_EQ(vec.size(), 3);
}

TEST(InplaceVectorTests, ForEachLoop) {
	engine::inplace_vector<int, 3> vec = { 1, 2, 3 };

	for (int& x : vec) {
		x++;
	}

	EXPECT_EQ(vec[0], 2);
	EXPECT_EQ(vec[1], 3);
	EXPECT_EQ(vec[2], 4);
}
