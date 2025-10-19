#pragma once

#include <engine/graphics/font_id.h>
#include <engine/graphics/image_id.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct InputDevices;
	class Renderer;
	class ResourceManager;
	class Window;

	struct RenderTestPage {
		enum {
			GeneralTest = 0,
			SpriteSheetTest = 1,
			Count,
		};
	};

	class RenderTestScreen {
	public:
		void initialize(ResourceManager* resources);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, FontID debug_font_id, IVec2 screen_resolution) const;

	private:
		void _draw_general_render_test(Renderer* renderer, FontID debug_font_id, IVec2 screen_resolution) const;
		void _draw_sprite_sheet_test(Renderer* renderer) const;

		int m_page = RenderTestPage::GeneralTest;
		ImageID m_clipping_image;
		ImageID m_transparency_image;
		ImageID m_spritesheet;
		uint8_t m_alpha = 255;
	};

} // namespace engine
