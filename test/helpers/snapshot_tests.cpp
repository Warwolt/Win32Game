#pragma once

#include <test/helpers/snapshot_tests.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace testing {

	enum class SnapshotTestResult {
		Passed,
		Failed,
		Updated,
	};

	struct SnapshotTestCase {
		std::string name;
		SnapshotTestResult result;
	};

	struct SnapshotTestSuite {
		std::string name;
		std::filesystem::path path;
		std::vector<SnapshotTestCase> tests;
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

	static std::string report_header_html(std::string title) {
		const auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		const std::string timestamp = std::format("{:%Y-%m-%d %H:%M}", now);
		return R"(<h1 style="margin-bottom: 0">)" + title + "</h1>" +
			R"(<p style="margin-top: 0">)" + timestamp + "</p>";
	}

	static std::string snapshot_stats_html(int num_passed, int num_failed) {
		return "<p>✅ Passed snapshots: " + std::to_string(num_passed) +
			"<br/>❌ Failed snapshots: " + std::to_string(num_failed) + "</p>";
	}

	static std::string snapshot_list_html(std::string title, const std::vector<SnapshotTestSuite>& suites, bool only_failed) {
		std::string list_html;
		list_html += "<ul>";
		for (const SnapshotTestSuite& suite : suites) {
			list_html += "<li>";
			list_html += R"(<a href=")" + suite.path.string() + "\">" + suite.name + "</a>";
			if (!suite.tests.empty()) list_html += "<ul>";
			for (const SnapshotTestCase& test : suite.tests) {
				if (only_failed && test.result != SnapshotTestResult::Failed) {
					continue;
				}
				list_html += "<li>";
				list_html += R"(<a href=")" + suite.path.string() + "#" + test.name + "\">" + test.name + "</a>";
				list_html += "</li>";
			}
			if (!suite.tests.empty()) list_html += "</ul>";
			list_html += "</li>";
		}
		list_html += "</ul>";

		return "<h2>" + title + "</h2>" +
			list_html;
	}

	static std::string snapshot_report_html() {
		int total_num_passed = 0;
		int total_num_failed = 0;
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					total_num_failed++;
				}
				else {
					total_num_passed++;
				}
			}
		}

		std::string html_body;
		html_body += report_header_html("Snapshot Test Report");
		html_body += snapshot_stats_html(total_num_passed, total_num_failed);
		html_body += snapshot_list_html("Failed snapshots", g_context.failed_suites, true);
		html_body += snapshot_list_html("Snapshots", g_context.all_suites, false);
		return std::format(html_template, html_body);
	}

	static std::string snapshot_suite_html(const SnapshotTestSuite& suite) {
		int total_num_passed = 0;
		int total_num_failed = 0;
		for (const SnapshotTestCase& test : suite.tests) {
			if (test.result == SnapshotTestResult::Failed) {
				total_num_failed++;
			}
			else {
				total_num_passed++;
			}
		}

		/* Add page header */
		std::string html_body;
		html_body += report_header_html("Snapshot Test Report: " + suite.name);
		html_body += "<a href=\"../index.html\">Back to summary</a>";
		html_body += snapshot_stats_html(total_num_passed, total_num_failed);

		/* List failed snapshots */
		html_body += "<h2>Failed snapshots</h2>";
		html_body += "<ul>";
		for (const SnapshotTestCase& test : suite.tests) {
			if (test.result == SnapshotTestResult::Failed) {
				html_body += "<li>";
				html_body += test.name;
				html_body += "</li>";
			}
		}
		html_body += "</ul>";

		/* Display snapshots */
		html_body += "<h2>Snapshots</h2>";
		for (const SnapshotTestCase& test : suite.tests) {
			/* Test name */
			html_body += std::format("<h3 id=\"{}\">", test.name);
			html_body += test.name;
			if (test.result == SnapshotTestResult::Failed) {
				html_body += " (failed)";
			}
			if (test.result == SnapshotTestResult::Updated) {
				html_body += " (updated)";
			}
			html_body += "</h3>";

			/* Snapshot */
			html_body += "<div style=\"display: flex; align-items: center\">";
			std::string snapshot_path = "../../test/snapshots/test_suite_name_1/test_case_name_1.png";
			std::string diff_path = "../../test/snapshots/test_suite_name_1/test_case_name_1.png";
			switch (test.result) {
				case SnapshotTestResult::Passed:
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", snapshot_path, "#00C500");
					break;

				case SnapshotTestResult::Failed:
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", snapshot_path, "#00C500");
					html_body += "<p style=\"margin: 1em\">➡️</p>";
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", diff_path, "#e33119");
					break;

				case SnapshotTestResult::Updated:
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", diff_path, "#00C500");
					html_body += "<p style=\"margin: 1em\">➡️</p>";
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", snapshot_path, "#00C500");
					break;
			}
			html_body += "</div>";
		}

		return std::format(html_template, html_body);
	}

	void initialize_snapshot_tests() {
		g_context.all_suites = {
			{
				"Test Suite Name 1",
				"test_suite_name_1/index.html",
				{
					{ "Apple", SnapshotTestResult::Failed },
					{ "Banana", SnapshotTestResult::Passed },
					{ "Coconut", SnapshotTestResult::Updated },
				},
			},
			{
				"Test Suite Name 2",
				"test_suite_name_2/index.html",
				{
					{ "Alice", SnapshotTestResult::Passed },
					{ "Bob", SnapshotTestResult::Passed },
				},
			},
			{
				"Test Suite Name 3",
				"test_suite_name_3/index.html",
				{
					{ "Axe", SnapshotTestResult::Passed },
					{ "Bow", SnapshotTestResult::Passed },
				},
			},
		};

		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					g_context.failed_suites.push_back(suite);
					break;
				}
			}
		}
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
