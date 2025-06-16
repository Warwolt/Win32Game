#include <gtest/gtest.h>

#include <engine/debug/moving_average.h>

using namespace engine;

TEST(MovingAverageTests, DefaultAverage_IsZero) {
	MovingAverage average;
	EXPECT_EQ(average.average(), 0.0f);
}

TEST(MovingAverageTests, SingleSample) {
	MovingAverage average = MovingAverage::with_sample_size(10);

	average.add(1.0f);

	EXPECT_EQ(average.average(), 1.0f);
}

TEST(MovingAverageTests, ThreeSamples) {
	MovingAverage average = MovingAverage::with_sample_size(10);

	average.add(1.0f);
	average.add(2.0f);
	average.add(3.0f);

	EXPECT_EQ(average.average(), 2.0f);
}

TEST(MovingAverageTests, TossesOutOldValues) {
	MovingAverage average = MovingAverage::with_sample_size(3);

	average.add(1.0f);
	average.add(2.0f);
	average.add(2.0f);
	average.add(2.0f);

	EXPECT_EQ(average.average(), 2.0f);
}
