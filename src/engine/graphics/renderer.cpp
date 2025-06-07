#include <engine/graphics/renderer.h>

namespace engine {

	void Renderer::clear_screen(RGBA color) {
		m_commands.push_back(ClearScreen { color });
	}

	void Renderer::draw_point(Vertex v1) {
		m_commands.push_back(DrawPoint { v1 });
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
		m_commands.push_back(DrawLine { v1, v2 });
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
			if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
				_put_point(bitmap, draw_point->v1);
			}
		}
		m_commands.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		for (int32_t y = 0; y < bitmap->width; y++) {
			for (int32_t x = 0; x < bitmap->width; x++) {
				bitmap->put(x, y, Pixel::from_rgba(color));
			}
		}
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1) {
		Pixel current_pixel = bitmap->get(v1.pos.x, v1.pos.y);
		Pixel new_pixel = current_pixel.lerp(Pixel::from_rgba(v1.color), v1.color.a / 255.0f);
		bitmap->put(v1.pos.x, v1.pos.y, new_pixel);
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2) {
	}

} // namespace engine
