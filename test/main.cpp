#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

constexpr char html_template[] =
R"(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>Snapshot Test Report</title>
  </head>
  <body>
	{}
  </body>
</html>)";


int main(int argc, char** argv) {
	/* Run tests */
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

	/* Produce snapshot report */
	std::ofstream report_file;
	std::filesystem::create_directory("snapshot_report");
	report_file.open("snapshot_report/index.html");
	report_file << std::format(html_template, "<p>Hello!</p>") << std::endl;
	report_file.close();

	return result;
}
