#pragma once

#include <string>

namespace testing {

    void report_failed_snapshot(std::string test_suite_name, std::string test_name);
    void report_passed_snapshot(std::string test_suite_name, std::string test_name);
    void report_updated_snapshot(std::string test_suite_name, std::string test_name);
    void generate_snapshot_report();

} // namespace testing
