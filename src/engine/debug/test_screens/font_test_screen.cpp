#include <engine/debug/test_screens/font_test_screen.h>

#include <engine/debug/assert.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace engine {

	void FontTestScreen::initialize() {
		std::vector<AnimationFrame<int>> frames = {
			{ 0, 1500ms },
			{ 1, 1500ms },
		};
		m_animation_system.add_animation(frames, { .looping = true });
	}

	void FontTestScreen::update(bool opened_now, const InputDevices& input) {
		if (opened_now) {
			DEBUG_ASSERT(m_animation_system.restart_animation(AnimationEntityID(1), AnimationID(1), input.time_now), "Couldn't start animation");
		}
		m_animation_system.update(input.time_now);
	}

	void FontTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		const int animation_index = m_animation_system.current_frame(AnimationEntityID(1));
		const int32_t font_size = 16;
		const int32_t font_ascent = 12;
		const int32_t text_box_width = animation_index == 0 ? screen_resolution.x / 2 : screen_resolution.x / 4 + 8;
		const int32_t text_box_height = animation_index == 0 ? font_ascent * 3 + 8 : font_ascent * 6;

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
