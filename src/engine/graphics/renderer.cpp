#include <engine/graphics/renderer.h>

#include <windows.h>

namespace engine {

	void Renderer::clear_screen() {
		m_draw_commands.push_back(ClearScreen {});
	}

	void Renderer::draw_pixel(int32_t x, int32_t y, Color color) {
		m_draw_commands.push_back(DrawPixel { x, y, color });
	}

	void Renderer::draw_line(IVec2 start, IVec2 end, Color color) {
		m_draw_commands.push_back(DrawLine { start, end, color });
	}

	void Renderer::render(engine::Bitmap* bitmap) {
		for (DrawCommand& command : m_draw_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				_clear_screen(bitmap);
			}
			if (auto* draw_pixel = std::get_if<DrawPixel>(&command)) {
				_put_pixel(bitmap, draw_pixel->x, draw_pixel->y, draw_pixel->color);
			}
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				_put_line(bitmap, draw_line->start, draw_line->end, draw_line->color);
			}
		}
		m_draw_commands.clear();
	}

	void Renderer::_clear_screen(engine::Bitmap* bitmap) {
		ZeroMemory(bitmap->data, bitmap->width * bitmap->height * sizeof(int32_t));
	}

	void Renderer::_put_pixel(engine::Bitmap* bitmap, int32_t x, int32_t y, Color color) {
		bitmap->put(x, y, BGRPixel { color.b, color.g, color.r });
	}

	void Renderer::_put_line(engine::Bitmap* bitmap, IVec2 start, IVec2 end, Color color) {
		// vertical line
		if (start.x == end.x) {
			int32_t y0 = min(start.y, end.y);
			int32_t y1 = max(start.y, end.y);
			for (int32_t y = y0; y <= y1; y++) {
				_put_pixel(bitmap, start.x, y, color);
			}
		}
		// sloped line
		else {
			// delta is the longer side of the triangle formed by the line
			// if dx is greater, x_step will be +1 or -1 and y_step will be the slope
			// if dy is greater, we flip it along the diagonal
			int32_t dx = end.x - start.x;
			int32_t dy = end.y - start.y;
			int32_t delta = max(std::abs(dx), std::abs(dy));
			float x_step = (float)dx / (float)delta;
			float y_step = (float)dy / (float)delta;
			for (int32_t i = 0; i <= delta; i++) {
				_put_pixel(bitmap, (int32_t)(start.x + i * x_step), (int32_t)(start.y + i * y_step), color);
			}
		}
	}

} // namespace engine
