#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/graphics/renderer.h>

namespace engine {

	void FontTestScreen::update(const InputDevices& /*input*/) {
	}

	void FontTestScreen::draw(Renderer* renderer) const {
		// FIXME: make `draw_text` optionally take a bounding box to draw the text within
		// The renderer has access to glyph sizes, so will know how to split up the text to fit the box.
		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 18 }, RGBA::white(), "THE QUICK BROWN FOX JUMPS OVER");
		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 18 + 16 }, RGBA::white(), "THE LAZY DOG.");

		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 58 }, RGBA::white(), "the quick brown fox jumps over");
		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 58 + 16 }, RGBA::white(), "the lazy dog.");

		// FIXME: can we add some kind of support for drawing "shaded" text?
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0 + 2, 98 + 2 }, RGBA::dark_purple(), "Sphinx of black");
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0, 98 }, RGBA::purple(), "Sphinx of black");
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0 + 2, 98 + 32 + 2 }, RGBA::dark_purple(), "quartz, judge");
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0, 98 + 32 }, RGBA::purple(), "quartz, judge");
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0 + 2, 98 + 64 + 2 }, RGBA::dark_purple(), "my vow!");
		renderer->draw_text(DEFAULT_FONT_ID, 32, { 0, 98 + 64 }, RGBA::purple(), "my vow!");
	}

} // namespace engine
