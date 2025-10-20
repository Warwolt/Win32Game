#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/graphics/renderer.h>

namespace engine {

	void FontTestScreen::update(const InputDevices& /*input*/) {
	}

	void FontTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		const int32_t font_size = 16;
		Rect text_rect = { 0, 18, screen_resolution.x / 2, font_size };
		renderer->draw_text(DEFAULT_FONT_ID, font_size, text_rect, RGBA::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.");
		renderer->draw_rect(text_rect, RGBA::green());

		renderer->draw_text(DEFAULT_FONT_ID, font_size, { 0, 58 }, RGBA::white(), "the quick brown fox jumps over");
		renderer->draw_text(DEFAULT_FONT_ID, font_size, { 0, 58 + 16 }, RGBA::white(), "the lazy dog.");

		// FIXME: can we add some kind of support for drawing "shaded" text?
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0 + 2, 98 + 2 }, RGBA::dark_purple(), "Sphinx of black");
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0, 98 }, RGBA::purple(), "Sphinx of black");
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0 + 2, 98 + 32 + 2 }, RGBA::dark_purple(), "quartz, judge");
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0, 98 + 32 }, RGBA::purple(), "quartz, judge");
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0 + 2, 98 + 64 + 2 }, RGBA::dark_purple(), "my vow!");
		renderer->draw_text(DEFAULT_FONT_ID, 2 * font_size, { 0, 98 + 64 }, RGBA::purple(), "my vow!");
	}

} // namespace engine
