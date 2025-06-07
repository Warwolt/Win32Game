#include <engine/graphics/renderer.h>

namespace engine {

	void Renderer::clear_screen(RGBA color) {
		m_commands.push_back(ClearScreen { color });
	}

	void Renderer::draw_point(IVec2 point, RGBA color) {
	}

	void Renderer::draw_line(Vertex start, Vertex end) {
	}

	void Renderer::draw_rect(Rect rect, RGBA color) {
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
	}

	void Renderer::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
	}

	void Renderer::render(Bitmap* bitmap) {
		for (const DrawCommand& command : m_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				_clear_screen(bitmap, clear_screen->color);
			}
		}
		m_commands.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		for (int32_t y = 0; y < bitmap->width; y++) {
			for (int32_t x = 0; x < bitmap->width; x++) {
				bitmap->put(x, y, BGRPixel::from_rgba(color));
			}
		}
	}

} // namespace engine
