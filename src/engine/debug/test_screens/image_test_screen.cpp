#include <engine/debug/test_screens/image_test_screen.h>

#include <engine/debug/assert.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace engine {

	void ImageTestScreen::initialize(ResourceManager* resources) {
		m_test_image = resources->load_image("assets/image/render_test/test_image.png");
		m_sprite_sheet = resources->load_image("assets/image/render_test/sprite_sheet.png");

		const Image& sprite_sheet = resources->image(m_sprite_sheet);
		m_sprite_sheet_size.width = sprite_sheet.width;
		m_sprite_sheet_size.height = sprite_sheet.height;

		std::vector<AnimationFrame<SpriteData>> sprite_animation = {
			{ { 0, false }, 430ms },
			{ { 1, false }, 430ms },
			{ { 0, false }, 430ms },
			{ { 1, false }, 430ms },

			{ { 3, false }, 430ms },
			{ { 2, false }, 430ms },
			{ { 3, false }, 430ms },
			{ { 2, false }, 430ms },

			{ { 4, false }, 430ms },
			{ { 5, false }, 430ms },
			{ { 4, false }, 430ms },
			{ { 5, false }, 430ms },

			{ { 3, true }, 430ms },
			{ { 2, true }, 430ms },
			{ { 3, true }, 430ms },
			{ { 2, true }, 430ms },
		};
		m_animation_entity_id = AnimationEntityID(1);
		m_animation_id = m_animation_system.add_animation(sprite_animation, { .looping = true });
	}

	void ImageTestScreen::update(bool opened_now, const InputDevices& input) {
		if (opened_now) {
			std::expected<void, AnimationError> result = m_animation_system.start_animation(m_animation_entity_id, m_animation_id, 0ms);
			DEBUG_ASSERT(result.has_value(), "Couldn't start animation");
		}

		m_animation_system.update(input.time_now);
	}

	void ImageTestScreen::draw(Renderer* renderer) const {
		const int32_t image_width = 32;
		const int32_t image_height = 32;

		/* Draw animated sprite */
		{
			const SpriteData& sprite = m_animation_system.current_frame(m_animation_entity_id);

			const IVec2 sprite_sheet_pos = { 0, 20 };
			const int32_t sprite_width = 16;
			const int32_t sprite_height = 16;
			const Rect sprite_clip_rect = Rect {
				.x = sprite.index * sprite_width,
				.y = 0,
				.width = sprite_width,
				.height = sprite_height,
			};

			/* Draw sprite sheet */
			RENDERER_LOG(renderer, "Draw sprite sheet");
			renderer->draw_image(m_sprite_sheet, sprite_sheet_pos);
			renderer->draw_rect(sprite_clip_rect + sprite_sheet_pos, RGBA::green());

			/* Draw sprite */
			RENDERER_LOG(renderer, "Draw sprite");
			const IVec2 sprite_pos = sprite_sheet_pos + IVec2 { m_sprite_sheet_size.width + sprite_width, 0 };
			renderer->draw_image(m_sprite_sheet, sprite_pos, { .clip = sprite_clip_rect, .flip_h = sprite.is_flipped });

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
				.x = sprite.index * scale * sprite_width,
				.y = 0,
				.width = scale * sprite_width,
				.height = scale * sprite_height,
			};
			renderer->draw_image_scaled(m_sprite_sheet, scaled_sprite_sheet_rect);
			renderer->draw_rect(scaled_sprite_clip_rect + scaled_sprite_sheet_rect.pos(), RGBA::green());

			/* Draw scaled sprite */
			RENDERER_LOG(renderer, "Draw sprite (scaled up)");
			const Rect scaled_sprite_rect = Rect {
				.x = scaled_sprite_sheet_rect.width + 12,
				.y = scaled_sprite_sheet_rect.y,
				.width = scale * sprite_width,
				.height = scale * sprite_height,
			};
			renderer->draw_image_scaled(m_sprite_sheet, scaled_sprite_rect, { .clip = sprite_clip_rect, .flip_h = sprite.is_flipped });
		}

		/* draw_image */
		{
			int column = 0;
			int row = 0;
			auto next_row = [&column, &row]() { column = 0; row++; };

			RENDERER_LOG(renderer, "Draw image");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) });

			RENDERER_LOG(renderer, "Draw image (flip horizontally)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .flip_h = true });

			RENDERER_LOG(renderer, "Draw image (flip vertically)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .flip_v = true });

			RENDERER_LOG(renderer, "Draw image (flip diagonally)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .flip_h = true, .flip_v = true });

			RENDERER_LOG(renderer, "Draw image (opacity 75%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .alpha = 0.75f });

			RENDERER_LOG(renderer, "Draw image (opacity 50%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .alpha = 0.50f });

			RENDERER_LOG(renderer, "Draw image (opacity 25%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .alpha = 0.25f });

			next_row();

			RENDERER_LOG(renderer, "Draw image (tint red 0%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .tint = RGBA::red().with_alpha(0.0f) });

			RENDERER_LOG(renderer, "Draw image (tint red 25%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .tint = RGBA::red().with_alpha(0.25f) });

			RENDERER_LOG(renderer, "Draw image (tint red 50%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .tint = RGBA::red().with_alpha(0.50f) });

			RENDERER_LOG(renderer, "Draw image (tint red 75%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .tint = RGBA::red().with_alpha(0.75f) });

			RENDERER_LOG(renderer, "Draw image (tint red 100%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .tint = RGBA::red().with_alpha(1.0f) });

			RENDERER_LOG(renderer, "Draw image (tint red 100%, opacity 75%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .alpha = 0.75f, .tint = RGBA::red().with_alpha(1.0f) });

			RENDERER_LOG(renderer, "Draw image (tint red 100%, opacity 50%)");
			renderer->draw_image(m_test_image, { column++ * (image_width + 2), 80 + row * (image_height + 2) }, { .alpha = 0.50f, .tint = RGBA::red().with_alpha(1.0f) });
		}

		/* draw_image_scaled */
		{
			int column = 0;
			RENDERER_LOG(renderer, "Draw image scaled");
			renderer->draw_image_scaled(m_test_image, { column++ * (2 * image_width + 2), 148, 2 * image_width, 2 * image_height });

			RENDERER_LOG(renderer, "Draw image scaled (flip diagonally, opacity 50%)");
			renderer->draw_image_scaled(m_test_image, { column++ * (2 * image_width + 2), 148, 2 * image_width, 2 * image_height }, { .flip_h = true, .flip_v = true, .alpha = 0.50f });

			RENDERER_LOG(renderer, "Draw image scaled (tint red 100%)");
			renderer->draw_image_scaled(m_test_image, { column++ * (2 * image_width + 2), 148, 2 * image_width, 2 * image_height }, { .tint = RGBA::red() });
		}
	}

} // namespace engine
