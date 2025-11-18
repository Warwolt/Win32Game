#pragma once

#include <engine/graphics/image.h>

#include <optional>
#include <string>

#define EXPECT_IMAGE_EQ_SNAPSHOT(image)                                                                                                           \
	do {                                                                                                                                          \
		const char* _test_suite_name = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();                                  \
		const char* _test_name = testing::UnitTest::GetInstance()->current_test_info()->name();                                                   \
		const engine::Image _current_image = (image);                                                                                             \
		if (std::optional<engine::Image> _snapshot_image = testing::load_snapshot(_test_suite_name, _test_name)) {                                \
			if (_current_image.pixels == _snapshot_image->pixels) {                                                                               \
				testing::report_passed_snapshot(_test_suite_name, _test_name);                                                                    \
			}                                                                                                                                     \
			else if (testing::should_update_snapshots()) {                                                                                        \
				printf("[   INFO   ] Updating snapshot file \"%s\"\n", testing::snapshot_filepath(_test_suite_name, _test_name).c_str());         \
				testing::report_updated_snapshot(_test_suite_name, _test_name);                                                                   \
				testing::save_snapshot_diff(_snapshot_image.value(), _test_suite_name, _test_name);                                               \
				testing::save_snapshot(_current_image, _test_suite_name, _test_name);                                                             \
			}                                                                                                                                     \
			else {                                                                                                                                \
				testing::report_failed_snapshot(_test_suite_name, _test_name);                                                                    \
				testing::save_snapshot_diff(_current_image, _test_suite_name, _test_name);                                                        \
				ADD_FAILURE() << "actual image in test did not match snapshot image, see test report for more info";                              \
			}                                                                                                                                     \
		}                                                                                                                                         \
		else {                                                                                                                                    \
			printf("[   INFO   ] No snapshot found, saving new file \"%s\"\n", testing::snapshot_filepath(_test_suite_name, _test_name).c_str()); \
			testing::report_passed_snapshot(_test_suite_name, _test_name);                                                                        \
			testing::save_snapshot(_current_image, _test_suite_name, _test_name);                                                                 \
		}                                                                                                                                         \
	} while (0)

namespace testing {

	std::string snapshot_directory(std::string test_suite_name);
	std::string snapshot_filepath(std::string test_suite_name, std::string test_name);
	bool should_update_snapshots();

	void report_failed_snapshot(std::string test_suite_name, std::string test_name);
	void report_passed_snapshot(std::string test_suite_name, std::string test_name);
	void report_updated_snapshot(std::string test_suite_name, std::string test_name);

	std::optional<engine::Image> load_snapshot(std::string test_suite_name, std::string test_name);
	void save_snapshot(const engine::Image& snapshot, std::string test_suite_name, std::string test_name);
	void save_snapshot_diff(const engine::Image& snapshot, std::string test_suite_name, std::string test_name);

	void initialize_snapshot_tests(int argc, char** argv);
	void generate_snapshot_report();

} // namespace testing
