#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	testing::generate_snapshot_report();
	return result;
}
