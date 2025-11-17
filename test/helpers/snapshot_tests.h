#pragma once

#include <engine/graphics/image.h>

#include <optional>
#include <string>

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
