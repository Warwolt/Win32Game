#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>

using namespace engine;

TEST(RendererTests, ClearScreen_Turquoise) {
	Renderer renderer = Renderer::with_bitmap(256, 240);
	ResourceManager resources;

	renderer.clear_screen({ 0, 127, 127, 255 });
	renderer.render(resources);

	// EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
	do {
		const char* _test_suite_name = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
		const char* _test_name = testing::UnitTest::GetInstance()->current_test_info()->name();
		const engine::Image _current_image = renderer.bitmap().to_image();
		if (std::optional<engine::Image> _snapshot_image = testing::load_snapshot(_test_suite_name, _test_name)) {
			if (_current_image.pixels == _snapshot_image->pixels) {
				testing::report_passed_snapshot(_test_suite_name, _test_name);
			}
			else if (testing::should_update_snapshots()) {
				printf("[   INFO   ] Updating snapshot file \"%s\"\n", testing::snapshot_filepath(_test_suite_name, _test_name).c_str());
				testing::report_updated_snapshot(_test_suite_name, _test_name);
				testing::save_snapshot_diff(_snapshot_image.value(), _test_suite_name, _test_name);
				testing::save_snapshot(_current_image, _test_suite_name, _test_name);
			}
			else {
				testing::report_failed_snapshot(_test_suite_name, _test_name);
				testing::save_snapshot_diff(_current_image, _test_suite_name, _test_name);
				ADD_FAILURE() << "actual image in test did not match snapshot image, see test report for more info";
			}
		}
		else {
			printf("[   INFO   ] No snapshot found, saving new file \"%s\"\n", testing::snapshot_filepath(_test_suite_name, _test_name).c_str());
			testing::report_passed_snapshot(_test_suite_name, _test_name);
			testing::save_snapshot(_current_image, _test_suite_name, _test_name);
		}

	} while (0);
}
