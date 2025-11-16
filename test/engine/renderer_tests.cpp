#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/graphics/renderer.h>

using namespace engine;

TEST(RendererTests, HelloWorld) {
    testing::report_failed_snapshot(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name(), testing::UnitTest::GetInstance()->current_test_info()->name());
	FAIL();
}
