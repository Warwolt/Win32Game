#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/graphics/renderer.h>

namespace engine {

	void FontTestScreen::update(const InputDevices& /*input*/) {
	}

	void FontTestScreen::draw(Renderer* renderer) const {
		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 18 }, RGBA::white(), "THE QUICK FOX JUMPS OVER");
		renderer->draw_text(DEFAULT_FONT_ID, 16, { 0, 18 + 16 }, RGBA::white(), "THE LAZY BROWN DOG");
	}

} // namespace engine
