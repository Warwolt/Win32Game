#pragma once

#include <test/helpers/snapshot_tests.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace testing {

	struct SnapshotTestSuite {
		std::string name;
		std::filesystem::path path;
	};

	struct SnapshotTestContext {
		std::vector<SnapshotTestSuite> all_suites;
		std::vector<SnapshotTestSuite> failed_suites;
	} g_context;

	constexpr char html_template[] = R"(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>Snapshot Test Report</title>
  </head>
  <body>
	{}
  </body>
</html>)";

	constexpr char header_template[] = R"(
<h1 style="margin-bottom: 0">{}</h1>
<p style="margin-top: 0">{}</p>
)";

	constexpr char snapshot_stats_template[] = R"(
<p>
  ✅ Passed snapshots: {}
  <br />
  ❌ Failed snapshots: {}
</p>
)";

	constexpr char snapshot_list_template[] = R"(
<h2>{}</h2>
<ul>{}</ul>
)";

	static std::string report_header_html(std::string title) {
		const auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		const std::string timestamp = std::format("{:%Y-%m-%d %H:%M}", now);
		return std::format(header_template, title, timestamp);
	}

	static std::string snapshot_stats_html(int num_passed, int num_failed) {
		return std::format(snapshot_stats_template, num_passed, num_failed);
	}

	static std::string snapshot_list_html(std::string title, const std::vector<SnapshotTestSuite>& suites) {
		std::string list_items;
		for (const SnapshotTestSuite& suite : suites) {
			list_items += std::format("<li><a href=\"{}\">{}</a></li>", suite.path.string(), suite.name);
		}

		return std::format(snapshot_list_template, title, list_items);
	}

	static std::string snapshot_report_html() {
		// FIXME: this should count the TEST CASES not the TEST SUITES !
		const int num_failed_snapshots = (int)g_context.failed_suites.size();
		const int num_passed_snapshots = (int)g_context.all_suites.size() - num_failed_snapshots;

		std::string html_body;
		html_body += report_header_html("Snapshot Test Report");
		html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);
		html_body += snapshot_list_html("Failed snapshots", g_context.failed_suites);
		html_body += snapshot_list_html("All snapshots", g_context.all_suites);
		return std::format(html_template, html_body);
	}

	static std::string snapshot_suite_html(const SnapshotTestSuite& suite) {
		std::string html_body;
		html_body += report_header_html(suite.name);
		return std::format(html_template, html_body);
	}

	void initialize_snapshot_tests() {
		g_context.all_suites = {
			{ "Test Suite Name 1", "test_suite_name_1/index.html" },
			{ "Test Suite Name 2", "test_suite_name_2/index.html" },
			{ "Test Suite Name 3", "test_suite_name_3/index.html" },
		};
		g_context.failed_suites = {
			{ "Test Suite Name 1", "test_suite_name_1/index.html" },
		};
	}

	void generate_snapshot_report() {
		/* Create report HTML file */
		std::filesystem::create_directory("snapshot_report");
		std::ofstream report_file;
		report_file.open("snapshot_report/index.html");
		report_file << snapshot_report_html() << std::endl;
		report_file.close();

		/* Create suite HTML files */
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			std::filesystem::path relative_path = suite.path.parent_path();
			std::filesystem::create_directory("snapshot_report" / relative_path);
			std::ofstream suite_file;
			suite_file.open("snapshot_report" / suite.path);
			suite_file << snapshot_suite_html(suite) << std::endl;
			suite_file.close();
		}
	}

} // namespace testing
