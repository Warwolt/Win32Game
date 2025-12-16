#include <game/ui/debug_screen/font_test_page.h>

#include <engine/debug/assert.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void FontDebugPage::update(bool opened_now, const engine::Input& input) {
		if (opened_now) {
			m_render_narrow = false;
		}
		if (input.keyboard.key_was_pressed_now(VK_DOWN) || input.keyboard.key_was_pressed_now(VK_UP)) {
			m_render_narrow = !m_render_narrow;
		}
	}

	void FontDebugPage::draw(engine::Renderer* renderer, engine::IVec2 screen_resolution) const {
		const int32_t font_size = 16;
		const int32_t font_ascent = 12;
		const int32_t text_box_width = m_render_narrow ? screen_resolution.x / 4 + 8 : screen_resolution.x / 2;
		const int32_t text_box_height = m_render_narrow ? font_ascent * 6 : font_ascent * 3 + 8;

		/* Uppercase, left aligned */
		{
			RENDERER_LOG(renderer, "Uppercase quick brown fox (left aligned)");
			engine::Rect text_rect = { 0, 18, text_box_width, text_box_height };
			engine::DrawTextOptions options = { .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.", options);
		}

		/* Lowercase, left aligned */
		{
			RENDERER_LOG(renderer, "Lowercase quick brown fox (left aligned)");
			engine::Rect text_rect = { screen_resolution.x / 2, 18, text_box_width, text_box_height };
			engine::DrawTextOptions options = { .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "the quick brown fox jumps over the lazy dog.", options);
		}

		/* Uppercase, centered */
		{
			RENDERER_LOG(renderer, "Uppercase quick brown fox (center aligned)");
			engine::Rect text_rect = { 0, 18 + 1 + text_box_height, text_box_width, text_box_height };
			engine::DrawTextOptions options = { .alignment = engine::Alignment::Center, .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.", options);
		}

		/* Lowercase, centered */
		{
			RENDERER_LOG(renderer, "Lowercase quick brown fox (center aligned)");
			engine::Rect text_rect = { screen_resolution.x / 2, 18 + 1 + text_box_height, text_box_width, text_box_height };
			engine::DrawTextOptions options = { .alignment = engine::Alignment::Center, .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "the quick brown fox jumps over the lazy dog.", options);
		}

		/* Uppercase, right aligned */
		{
			RENDERER_LOG(renderer, "Uppercase quick brown fox (right aligned)");
			engine::Rect text_rect = { 0, 18 + 2 * (1 + text_box_height), text_box_width, text_box_height };
			engine::DrawTextOptions options = { .alignment = engine::Alignment::Right, .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.", options);
		}

		/* Lowercase, right aligned */
		{
			RENDERER_LOG(renderer, "Lowercase quick brown fox (right aligned)");
			engine::Rect text_rect = { screen_resolution.x / 2, 18 + 2 * (1 + text_box_height), text_box_width, text_box_height };
			engine::DrawTextOptions options = { .alignment = engine::Alignment::Right, .debug_draw_box = true };
			renderer->draw_text(engine::DEFAULT_FONT_ID, font_size, text_rect, engine::Color::white(), "the quick brown fox jumps over the lazy dog.", options);
		}
	}

} // namespace game
