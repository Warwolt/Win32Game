#pragma once

#include <test/helpers/snapshot_tests.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace testing {

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
<h1 style="margin-bottom: 0">Snapshot Test Report</h1>
<p style="margin-top: 0">{}</p>
)";

	constexpr char snapshot_stats_template[] = R"(
<p>
  ✅ Passed snapshots: {}
  <br />
  ❌ Failed snapshots: {}
</p>
)";

	constexpr char failed_snapshots_template[] = R"(
<h2>Failed snapshots</h2>
<ul>{}</ul>
)";

	constexpr char all_snapshots_template[] = R"(
<h2>All snapshots</h2>
<ul>{}</ul>
)";

	static std::string report_header_html() {
		const auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		const std::string timestamp = std::format("{:%Y-%m-%d %H:%M}", now);
		return std::format(header_template, timestamp);
	}

	static std::string snapshot_stats_html() {
		const int passed_snapshots = 0;
		const int failed_snapshots = 0;
		return std::format(snapshot_stats_template, passed_snapshots, failed_snapshots);
	}

	static std::string failed_snapshots_html() {
		const std::vector<std::string> failed_suite_names = {
			"Test suite name 1",
		};

		std::string list_items;
		for (const std::string& name : failed_suite_names) {
			list_items += std::format("<li>{}</li>", name);
		}

		return std::format(failed_snapshots_template, list_items);
	}

	static std::string all_snapshots_html() {
		const std::vector<std::string> all_suite_names = {
			"Test suite name 1",
			"Test suite name 2",
			"Test suite name 3",
		};

		std::string list_items;
		for (const std::string& name : all_suite_names) {
			list_items += std::format("<li>{}</li>", name);
		}

		return std::format(all_snapshots_template, list_items);
	}

	static std::string snapshot_report_html() {
		std::string html_body;
		html_body += report_header_html();
		html_body += snapshot_stats_html();
		html_body += failed_snapshots_html();
		html_body += all_snapshots_html();
		return std::format(html_template, html_body);
	}

	void generate_snapshot_report() {
		std::ofstream report_file;
		std::filesystem::create_directory("snapshot_report");
		report_file.open("snapshot_report/index.html");
		report_file << snapshot_report_html() << std::endl;
		report_file.close();
	}

} // namespace testing
