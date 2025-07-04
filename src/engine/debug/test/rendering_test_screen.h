#pragma once

#include <engine/graphics/font_id.h>
#include <engine/graphics/image_id.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct InputDevices;
	class Renderer;
	class ResourceManager;
	class Window;

	class RenderingTestScreen {
	public:
		void initialize(ResourceManager* resources);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, FontID debug_font_id, IVec2 screen_resolution) const;

	private:
		ImageID m_render_test_image_id;
		uint8_t m_alpha = 255;
		int32_t m_font_size = 16;
	};

} // namespace engine
