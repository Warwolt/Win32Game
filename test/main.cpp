#include <gtest/gtest.h>

#include <engine/utility/string_utility.h>
#include <test/helpers/snapshot_tests.h>

int main(int argc, char** argv) {
	bool gtest_filter_enabled = false;
	for (int i = 0; i < argc; i++) {
		if (engine::string_starts_with(argv[i], "--gtest_filter=")) {
			gtest_filter_enabled = true;
		}
	}

	testing::InitGoogleTest(&argc, argv);
	testing::initialize_snapshot_tests(argc, argv, gtest_filter_enabled);

	int result = RUN_ALL_TESTS();
	testing::generate_snapshot_report();

	return result;
}
