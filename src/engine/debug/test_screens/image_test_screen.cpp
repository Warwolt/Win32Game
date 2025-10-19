#include <engine/debug/test_screens/image_test_screen.h>

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

	void ImageTestScreen::initialize(ResourceManager* resources) {
		m_clipping_image = resources->load_image("assets/image/render_test/clipping.png");
		m_transparency_image = resources->load_image("assets/image/render_test/transparency.png");
		m_sprite_sheet = resources->load_image("assets/image/render_test/sprite_sheet.png");

		const Image& sprite_sheet = resources->image(m_sprite_sheet);
		m_sprite_sheet_size.width = sprite_sheet.width;
		m_sprite_sheet_size.height = sprite_sheet.height;

		m_animation_frames = {
			{ 0, false },
			{ 1, false },
			{ 0, false },
			{ 1, false },

			{ 3, false },
			{ 2, false },
			{ 3, false },
			{ 2, false },

			{ 4, false },
			{ 5, false },
			{ 4, false },
			{ 5, false },

			{ 3, true },
			{ 2, true },
			{ 3, true },
			{ 2, true },
		};
	}

	void ImageTestScreen::update(const InputDevices& input) {
		if ((input.time_now - m_last_sprite_sheet_frame).in_milliseconds() >= 430) {
			m_last_sprite_sheet_frame = input.time_now;
			m_animation_index = (m_animation_index + 1) % m_animation_frames.size();
		}
	}

	void ImageTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		/* Draw animated sprite */
		{
			const IVec2 sprite_sheet_pos = { 0, 20 };
			const int32_t sprite_width = 16;
			const int32_t sprite_height = 16;
			const Rect sprite_clip_rect = Rect {
				.x = m_animation_frames[m_animation_index].index * sprite_width,
				.y = 0,
				.width = sprite_width,
				.height = sprite_height,
			};
			const bool is_flipped = m_animation_frames[m_animation_index].flipped;

			/* Draw sprite sheet */
			RENDERER_LOG(renderer, "Draw sprite sheet");
			renderer->draw_image(m_sprite_sheet, sprite_sheet_pos);
			renderer->draw_rect(sprite_clip_rect + sprite_sheet_pos, RGBA::green());

			/* Draw sprite */
			RENDERER_LOG(renderer, "Draw sprite");
			const IVec2 sprite_pos = sprite_sheet_pos + IVec2 { m_sprite_sheet_size.width + sprite_width, 0 };
			renderer->draw_image(m_sprite_sheet, sprite_pos, sprite_clip_rect, { .flip_h = is_flipped });

			/* Draw sprite sheet scaled */
			RENDERER_LOG(renderer, "Draw sprite sheet (scaled up)");
			const int scale = 2;
			const Rect scaled_sprite_sheet_rect = {
				.x = sprite_sheet_pos.x,
				.y = sprite_sheet_pos.y + sprite_height,
				.width = (int32_t)(m_sprite_sheet_size.width * scale),
				.height = (int32_t)(m_sprite_sheet_size.height * scale),
			};
			const Rect scaled_sprite_clip_rect = Rect {
				.x = m_animation_frames[m_animation_index].index * scale * sprite_width,
				.y = 0,
				.width = scale * sprite_width,
				.height = scale * sprite_height,
			};
			renderer->draw_image_scaled(m_sprite_sheet, scaled_sprite_sheet_rect);
			renderer->draw_rect(scaled_sprite_clip_rect + scaled_sprite_sheet_rect.pos(), RGBA::green());

			/* Draw scaled sprite */
			RENDERER_LOG(renderer, "Draw sprite (scaled up)");
			const Rect scaled_sprite_rect = Rect {
				.x = scaled_sprite_sheet_rect.width + scale * sprite_width,
				.y = scaled_sprite_sheet_rect.y,
				.width = scale * sprite_width,
				.height = scale * sprite_height,
			};
			renderer->draw_image_scaled(m_sprite_sheet, scaled_sprite_rect, sprite_clip_rect, { .flip_h = is_flipped });
		}
	}

} // namespace engine
