#include <engine/ui/debug_screen/debug_screen.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>

#include <windows.h>

#include <algorithm>
#include <format>

namespace engine {

	constexpr int FONT_SIZE = 16;

	void DebugScreen::initialize(ResourceManager* resources, CommandList* /*commands*/) {
		m_image_test_page.initialize(resources);
		m_font_test_page.initialize();
	}

	void DebugScreen::update(const Input& input, CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->pop_screen();
		}

		/* Update current page */
		if (m_page == DebugScreenPage::GeometryTest) {
			m_geometry_test_page.update(input);
		}
		if (m_page == DebugScreenPage::ImageTest) {
			m_image_test_page.update(m_just_changed_page, input);
		}
		if (m_page == DebugScreenPage::FontTest) {
			m_font_test_page.update(m_just_changed_page, input);
		}

		/* Switch page */
		m_just_changed_page = false;
		if (input.keyboard.key_was_pressed_now(VK_RIGHT)) {
			m_page = (DebugScreenPage::Count + m_page + 1) % DebugScreenPage::Count;
			m_just_changed_page = true;
		}
		if (input.keyboard.key_was_pressed_now(VK_LEFT)) {
			m_page = (DebugScreenPage::Count + m_page - 1) % DebugScreenPage::Count;
			m_just_changed_page = true;
		}
	}

	void DebugScreen::draw(Renderer* renderer) const {
		CPUProfilingScope_Engine();
		const char* title = "unknown";
		/* Render page */
		if (m_page == DebugScreenPage::GeometryTest) {
			title = "draw geometry";
			m_geometry_test_page.draw(renderer, renderer->screen_resolution());
		}
		if (m_page == DebugScreenPage::ImageTest) {
			title = "draw images";
			m_image_test_page.draw(renderer);
		}
		if (m_page == DebugScreenPage::FontTest) {
			title = "draw text";
			m_font_test_page.draw(renderer, renderer->screen_resolution());
		}
		/* Render page title */
		renderer->draw_text(DEFAULT_FONT_ID, FONT_SIZE, { 0, 0 }, RGBA::white(), std::format("test page {}/{}: {}", (int)m_page + 1, (int)DebugScreenPage::Count, title));
	}

} // namespace engine
