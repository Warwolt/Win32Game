#include <test/helpers/snapshot_tests.h>

#include <engine/debug/assert.h>

#include <stb_image/stb_image_write.h>

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

	static std::string snapshot_stats_html(int num_passed_snapshots, int num_failed_snapshots) {
		return "<p>✅ Passed snapshots: " + std::to_string(num_passed_snapshots) +
			"<br/>❌ Failed snapshots: " + std::to_string(num_failed_snapshots) + "</p>";
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
		int num_passed_snapshots = 0;
		int num_failed_snapshots = 0;
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					num_failed_snapshots++;
				}
				else {
					num_passed_snapshots++;
				}
			}
		}
		const int num_snapshots = num_passed_snapshots + num_failed_snapshots;
		const int num_suites = (int)g_context.all_suites.size();

		std::string html_body;
		html_body += report_header_html("Snapshot Test Report");
		html_body += "<p>" +
			std::to_string(num_snapshots) + " snapshot" + (num_snapshots == 1 ? "" : "s") + " from " +
			std::to_string(num_suites) + " test suite" + (num_suites == 1 ? "" : "s") + " ran." +
			"</p>";
		html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);
		if (!g_context.failed_suites.empty()) {
			html_body += snapshot_list_html("Failures", g_context.failed_suites, true);
		}
		html_body += snapshot_list_html("Snapshots", g_context.all_suites, false);
		return std::format(html_template, html_body);
	}

	static std::string snapshot_suite_html(const SnapshotTestSuite& suite) {
		int num_passed_snapshots = 0;
		int num_failed_snapshots = 0;
		for (const SnapshotTestCase& test : suite.tests) {
			if (test.result == SnapshotTestResult::Failed) {
				num_failed_snapshots++;
			}
			else {
				num_passed_snapshots++;
			}
		}

		/* Add page header */
		std::string html_body;
		html_body += report_header_html("Snapshot Test Report: " + suite.name);
		html_body += R"(<a href="../index.html">Back to summary</a>)";
		html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);

		/* List failed snapshots */
		if (num_failed_snapshots > 0) {
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

	std::string snapshot_directory(std::string test_suite_name) {
		return "test/snapshots/" + test_suite_name;
	}

	std::string snapshot_filepath(std::string test_suite_name, std::string test_name) {
		return snapshot_directory(test_suite_name) + "/" + test_name + ".png";
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

	std::optional<engine::Image> load_snapshot(std::string test_suite_name, std::string test_name) {
		return engine::Image::from_path(snapshot_filepath(test_suite_name, test_name));
	}

	void save_snapshot(const engine::Image& snapshot, std::string test_suite_name, std::string test_name) {
		std::filesystem::create_directories(snapshot_directory(test_suite_name));
		int _result = stbi_write_png(snapshot_filepath(test_suite_name, test_name).c_str(), snapshot.width, snapshot.height, 4, snapshot.pixels.data(), snapshot.width * 4);
		DEBUG_ASSERT(_result != 0, "stbi_write_png failed");
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
