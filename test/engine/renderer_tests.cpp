#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/graphics/renderer.h>

using namespace engine;

#define EXPECT_SNAPSHOT_EQ(expr)                                                                                                                                                  \
	engine::Bitmap snapshot = {};                                                                                                                                                 \
	if ((expr) == snapshot) {                                                                                                                                                     \
		testing::report_passed_snapshot(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name(), testing::UnitTest::GetInstance()->current_test_info()->name()); \
	}                                                                                                                                                                             \
	else {                                                                                                                                                                        \
		testing::report_failed_snapshot(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name(), testing::UnitTest::GetInstance()->current_test_info()->name()); \
		FAIL() << #expr << " did not match snapshot, see test report for more info.";                                                                                             \
	}

TEST(RendererTests, HelloWorld) {
	Renderer renderer = Renderer::with_bitmap(256, 240);

	// run drawing stuff here

	EXPECT_SNAPSHOT_EQ(renderer.bitmap());
}
