#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace engine {

	void FontTestScreen::update(const InputDevices& input) {
		// FIXME: add support for "ms" operator for Time and a comparison operator
		// Maybe we can just re-use std::chrono stuff here?
		// if (input.time_now - m_last_animation_frame >= 1000ms) {

		if ((input.time_now - m_last_animation_frame).in_milliseconds() >= 2000) {
			m_last_animation_frame = input.time_now;
			m_animation_index = (m_animation_index + 1) % 2;
		}
	}

	void FontTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		const int32_t font_size = 16;
		const int32_t font_ascent = 12;
		const int32_t text_box_width = m_animation_index == 0 ? screen_resolution.x / 2 : screen_resolution.x / 4 + 8;
		const int32_t text_box_height = m_animation_index == 0 ? font_ascent * 3 + 8 : font_ascent * 6;

		/* Uppercase */
		{
			RENDERER_LOG(renderer, "Uppercase quick brown fox");
			Rect text_rect = { 0, 18, text_box_width, text_box_height };
			renderer->draw_text(DEFAULT_FONT_ID, font_size, text_rect, RGBA::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.");
			renderer->draw_rect(text_rect, RGBA::green());
		}

		/* Lowercase */
		{
			RENDERER_LOG(renderer, "Lowercase quick brown fox");
			Rect text_rect = { screen_resolution.x / 2, 18, text_box_width, text_box_height };
			renderer->draw_text(DEFAULT_FONT_ID, font_size, text_rect, RGBA::white(), "the quick brown fox jumps over the lazy dog.");
			renderer->draw_rect(text_rect, RGBA::green());
		}

		/* Big shaded font */
		{
			RENDERER_LOG(renderer, "Big shaded sphinx of black quartz");
			Rect text_rect = { 0, 112, screen_resolution.x - 10, screen_resolution.y };
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 2, 2 }, RGBA::dark_purple(), "Sphinx of black quartz, judge my vow!");
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 1, 1 }, RGBA::dark_purple(), "Sphinx of black quartz, judge my vow!");
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 0, 0 }, RGBA::purple(), "Sphinx of black quartz, judge my vow!");
		}
	}

} // namespace engine
