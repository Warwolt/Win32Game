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

	static std::string snapshot_stats_html(int num_passed, int num_failed, int num_suites) {
		int total_num = num_passed + num_failed;
		return "<p>" +
			std::to_string(total_num) + " snapshot" + (total_num == 1 ? "" : "s") + " from " +
			std::to_string(num_suites) + " test suite" + (num_suites == 1 ? "" : "s") + " ran." +
			"</p>" +
			"<p>✅ Passed snapshots: " + std::to_string(num_passed) +
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
		html_body += snapshot_stats_html(total_num_passed, total_num_failed, (int)g_context.all_suites.size());
		if (!g_context.failed_suites.empty()) {
			html_body += snapshot_list_html("Failures", g_context.failed_suites, true);
		}
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
		html_body += R"(<a href="../index.html">Back to summary</a>)";
		html_body += snapshot_stats_html(total_num_passed, total_num_failed, 1);

		/* List failed snapshots */
		if (total_num_failed > 0) {
			html_body += "<h2>Failures</h2>";
			html_body += "<ul>";
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					html_body += "<li>";
					html_body += std::format(R"(<a href="#{}">{}</a>)", test.name, test.name);
					html_body += "</li>";
				}
			}
			html_body += "</ul>";
		}

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
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", diff_path, "#C5C500");
					html_body += "<p style=\"margin: 1em\">➡️</p>";
					html_body += std::format("<img src=\"{}\" style=\"border: 2px solid {}\">", snapshot_path, "#00C500");
					break;
			}
			html_body += "</div>";
		}

		return std::format(html_template, html_body);
	}

	static void report_snapshot(std::string test_suite_name, std::string test_name, SnapshotTestResult test_result) {
		SnapshotTestCase test_case = {
			.name = test_name,
			.result = test_result,
		};
		auto it = std::find_if(g_context.all_suites.begin(), g_context.all_suites.end(), [&](const SnapshotTestSuite& suite) { return suite.name == test_suite_name; });
		if (it == g_context.all_suites.end()) {
			g_context.all_suites.push_back(SnapshotTestSuite {
				.name = test_suite_name,
				.path = test_suite_name + "/index.html",
				.tests = { test_case },
			});
		}
		else {
			it->tests.push_back(test_case);
		}
	}

	void report_failed_snapshot(std::string test_suite_name, std::string test_name) {
		report_snapshot(test_suite_name, test_name, SnapshotTestResult::Failed);
	}

	void report_passed_snapshot(std::string test_suite_name, std::string test_name) {
		report_snapshot(test_suite_name, test_name, SnapshotTestResult::Passed);
	}

	void report_updated_snapshot(std::string test_suite_name, std::string test_name) {
		report_snapshot(test_suite_name, test_name, SnapshotTestResult::Updated);
	}

	void generate_snapshot_report() {
		/* Copy over failed tests */
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					g_context.failed_suites.push_back(suite);
					break;
				}
			}
		}

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

		if (!g_context.all_suites.empty()) {
			printf("\nSnapshot test report updated: %s\n", "./snapshot_report/index.html");
		}
	}

} // namespace testing
