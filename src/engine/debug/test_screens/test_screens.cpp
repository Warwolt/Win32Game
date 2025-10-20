#include <engine/debug/test_screens/test_screens.h>

#include <engine/debug/profiling.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>

#include <windows.h>

#include <algorithm>
#include <array>
#include <format>

namespace engine {

	constexpr int FONT_SIZE = 16;

	void TestScreens::initialize(ResourceManager* resources, int initial_page) {
		m_page = std::clamp(initial_page, 0, (int)TestScreenPage::Count);
		m_image_test_screen.initialize(resources);
	}

	void TestScreens::update(const InputDevices& input) {
		/* Update current page */
		if (m_page == TestScreenPage::GeometryTest) {
			m_geometry_test_screen.update(input);
		}
		if (m_page == TestScreenPage::ImageTest) {
			m_image_test_screen.update(m_just_changed_page, input);
		}
		if (m_page == TestScreenPage::FontTest) {
			m_font_test_screen.update(m_just_changed_page, input);
		}

		/* Switch page */
		m_just_changed_page = false;
		if (input.keyboard.key_was_pressed_now(VK_RIGHT)) {
			m_page = (TestScreenPage::Count + m_page + 1) % TestScreenPage::Count;
			m_just_changed_page = true;
		}
		if (input.keyboard.key_was_pressed_now(VK_LEFT)) {
			m_page = (TestScreenPage::Count + m_page - 1) % TestScreenPage::Count;
			m_just_changed_page = true;
		}
	}

	void TestScreens::draw(Renderer* renderer, IVec2 screen_resolution) const {
		CPUProfilingScope_Engine();
		const char* title = "unknown";
		/* Render page */
		if (m_page == TestScreenPage::GeometryTest) {
			title = "draw geometry";
			m_geometry_test_screen.draw(renderer, screen_resolution);
		}
		if (m_page == TestScreenPage::ImageTest) {
			title = "draw images";
			m_image_test_screen.draw(renderer);
		}
		if (m_page == TestScreenPage::FontTest) {
			title = "draw text";
			m_font_test_screen.draw(renderer, screen_resolution);
		}
		/* Render page title */
		renderer->draw_text(DEFAULT_FONT_ID, FONT_SIZE, { 0, 0 }, RGBA::white(), std::format("test page {}/{}: {}", (int)m_page + 1, (int)TestScreenPage::Count, title));
	}

} // namespace engine
