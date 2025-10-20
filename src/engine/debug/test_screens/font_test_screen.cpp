#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/graphics/renderer.h>

namespace engine {

	void FontTestScreen::update(const InputDevices& /*input*/) {
	}

	void FontTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		const int32_t font_size = 16;
		const int32_t font_ascent = 12;

		/* Uppercase */
		{
			RENDERER_LOG(renderer, "Uppercase quick brown fox");
			Rect text_rect = { 0, 18, screen_resolution.x / 2, font_ascent * 3 + 8 };
			renderer->draw_text(DEFAULT_FONT_ID, font_size, text_rect, RGBA::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.");
			renderer->draw_rect(text_rect, RGBA::green());
		}

		/* Lowercase */
		{
			RENDERER_LOG(renderer, "Lowercase quick brown fox");
			Rect text_rect = { screen_resolution.x / 2, 18, screen_resolution.x / 2, font_ascent * 3 + 8 };
			renderer->draw_text(DEFAULT_FONT_ID, font_size, text_rect, RGBA::white(), "the quick brown fox jumps over the lazy dog.");
			renderer->draw_rect(text_rect, RGBA::green());
		}

		/* Big shaded font */
		{
			RENDERER_LOG(renderer, "Big shaded sphinx of black quartz");
			Rect text_rect = { 0, 98, screen_resolution.x - 10, screen_resolution.y };
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 2, 2 }, RGBA::dark_purple(), "Sphinx of black quartz, judge my vow!");
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 1, 1 }, RGBA::dark_purple(), "Sphinx of black quartz, judge my vow!");
			renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, text_rect + IVec2 { 0, 0 }, RGBA::purple(), "Sphinx of black quartz, judge my vow!");
		}
	}

} // namespace engine
