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

	void TestScreens::initialize(ResourceManager* resources) {
		m_image_test_screen.initialize(resources);
	}

	void TestScreens::update(const InputDevices& input) {
		/* Update current page */
		if (m_page == RenderTestPage::GeometryTest) {
			m_geometry_test_screen.update(input);
		}
		if (m_page == RenderTestPage::ImageTest) {
			m_image_test_screen.update(input);
		}
		if (m_page == RenderTestPage::FontTest) {
			m_font_test_screen.update(input);
		}

		/* Switch page */
		if (input.keyboard.key_was_pressed_now(VK_RIGHT)) {
			m_page = (RenderTestPage::Count + m_page + 1) % RenderTestPage::Count;
		}
		if (input.keyboard.key_was_pressed_now(VK_LEFT)) {
			m_page = (RenderTestPage::Count + m_page - 1) % RenderTestPage::Count;
		}
	}

	void TestScreens::draw(Renderer* renderer, IVec2 screen_resolution) const {
		CPUProfilingScope_Engine();
		const char* title = "unknown";
		/* Render page */
		if (m_page == RenderTestPage::GeometryTest) {
			title = "draw geometry";
			m_geometry_test_screen.draw(renderer, screen_resolution);
		}
		if (m_page == RenderTestPage::ImageTest) {
			title = "draw images";
			m_image_test_screen.draw(renderer);
		}
		if (m_page == RenderTestPage::FontTest) {
			title = "draw text";
			m_font_test_screen.draw(renderer);
		}
		/* Render page title */
		renderer->draw_text(DEFAULT_FONT_ID, FONT_SIZE, { 0, 0 }, RGBA::white(), std::format("test page {}/{}: {}", (int)m_page + 1, (int)RenderTestPage::Count, title));
	}

} // namespace engine
