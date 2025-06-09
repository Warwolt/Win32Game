#include <engine/graphics/renderer.h>

#include <cmath>
#include <unordered_set>

namespace engine {

	static std::vector<IVec2> circle_octant_points(int32_t radius) {
		/* Compute points in 2nd octant */
		// We utilize that y value increases monotonously in the first octant,
		// and check which pixel is closest to the radius at a given x-value.
		//               90°
		//         , - ~ ~ ~ - ,
		//     , '       |       ' , 45°
		//   ,           |       ⟋   ,
		//  ,            |    ⟋       ,
		// ,             | ⟋           ,
		// ,             o             ,
		// ,                           ,
		//  ,                         ,
		//   ,                       ,
		//     ,                  , '
		//       ' - , _ _ _ ,  '
		std::vector<IVec2> octant_points;
		IVec2 point = { 0, radius };
		while (point.x <= point.y) {
			octant_points.push_back(point);

			int32_t midpoint_x = point.x + 1;
			float midpoint_y = point.y - 0.5f;
			if (pow(midpoint_x, 2) + pow(midpoint_y, 2) > pow(radius, 2)) {
				point.y -= 1;
			}
			point.x += 1;
		}
		return octant_points;
	}

	void Renderer::clear_screen(RGBA color) {
	}

	void Renderer::draw_point(Vertex v1) {
		m_batches.push_back(CommandBatch {
			.rect = Rect {
				v1.pos.x,
				v1.pos.y,
				1,
				1,
			},
			.commands = {
				DrawPoint { v1 },
			},
		});
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
		int32_t min_x = std::min(v1.pos.x, v2.pos.x);
		int32_t min_y = std::min(v1.pos.y, v2.pos.y);
		int32_t max_x = std::max(v1.pos.x, v2.pos.x);
		int32_t max_y = std::max(v1.pos.y, v2.pos.y);
		m_batches.push_back(CommandBatch {
			.rect = Rect {
				min_x,
				min_y,
				max_x - min_x + 1,
				max_y - min_y + 1,
			},
			.commands = {
				DrawLine { v1, v2 },
			},
		});
	}

	void Renderer::draw_rect(Rect rect, RGBA color) {
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
	}

	void Renderer::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
	}

	void Renderer::draw_triangle(Vertex v1, Vertex v2, Vertex v3) {
	}

	void Renderer::draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3) {
	}

	void Renderer::render(Bitmap* bitmap) {
		m_scratchpad.resize(bitmap->width(), bitmap->height());

		for (const CommandBatch& batch : m_batches) {
			/* Clear scratch pad area */
			for (int32_t y = batch.rect.y; y < batch.rect.y + batch.rect.height; y++) {
				for (int32_t x = batch.rect.x; x < batch.rect.x + batch.rect.width; x++) {
					m_scratchpad.put(x, y, Pixel { 0, 0, 0, 0 });
				}
			}

			/* Draw onto scratch pad */
			for (const DrawCommand& command : batch.commands) {
				if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
					_clear_screen(bitmap, clear_screen->color);
				}
				if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
					_put_point(&m_scratchpad, draw_point->v1);
				}
				if (auto* draw_line = std::get_if<DrawLine>(&command)) {
					_put_line(&m_scratchpad, draw_line->v1, draw_line->v2);
				}
				if (auto* draw_triangle = std::get_if<DrawTriangle>(&command)) {
					_put_triangle(&m_scratchpad, draw_triangle->v1, draw_triangle->v2, draw_triangle->v3);
				}
			}

			/* Draw scratch pad onto bitmap */
			for (int32_t y = batch.rect.y; y < batch.rect.y + batch.rect.height; y++) {
				for (int32_t x = batch.rect.x; x < batch.rect.x + batch.rect.width; x++) {
					Pixel scratchpad_pixel = m_scratchpad.get(x, y);
					Pixel bitmap_pixel = bitmap->get(x, y);
					uint8_t alpha = scratchpad_pixel.padding;
					bitmap->put(x, y, bitmap_pixel.lerp(scratchpad_pixel, alpha / 255.0f));
				}
			}
		}
		m_batches.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		for (int32_t y = 0; y < bitmap->height(); y++) {
			for (int32_t x = 0; x < bitmap->width(); x++) {
				bitmap->put(x, y, Pixel::from_rgb(color));
			}
		}
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1) {
		bitmap->put(v1.pos.x, v1.pos.y, Pixel { .b = v1.color.b, .g = v1.color.g, .r = v1.color.r, .padding = v1.color.a });
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2) {
		// vertical line
		if (v1.pos.x == v2.pos.x) {
			int32_t y0 = std::min(v1.pos.y, v2.pos.y);
			int32_t y1 = std::max(v1.pos.y, v2.pos.y);
			for (int32_t y = y0; y <= y1; y++) {
				IVec2 pos = IVec2 { v1.pos.x, y };
				float t = (float)(pos.y - v1.pos.y) / (float)(v2.pos.y - v1.pos.y);
				Vertex vertex = { .pos = pos, .color = RGBA::lerp(v1.color, v2.color, t) };
				_put_point(bitmap, vertex);
			}
		}
		// sloped line
		else {
			// big_delta is the longer side of the triangle formed by the line
			// if dx is greater, x_step will be +1 or -1 and y_step will be the slope
			// if dy is greater, we flip it along the diagonal
			int32_t dx = v2.pos.x - v1.pos.x;
			int32_t dy = v2.pos.y - v1.pos.y;
			int32_t abs_dx = std::abs(dx);
			int32_t abs_dy = std::abs(dy);
			int32_t big_delta = std::max(abs_dx, abs_dy);
			float x_step = (float)dx / (float)big_delta;
			float y_step = (float)dy / (float)big_delta;
			for (int32_t i = 0; i <= big_delta; i++) {
				IVec2 pos = IVec2 { .x = (int32_t)(v1.pos.x + i * x_step), .y = (int32_t)(v1.pos.y + i * y_step) };
				float t = abs_dx > abs_dy
					? (float)(pos.x - v1.pos.x) / (float)(v2.pos.x - v1.pos.x)
					: (float)(pos.y - v1.pos.y) / (float)(v2.pos.y - v1.pos.y);
				Vertex vertex = { .pos = pos, .color = RGBA::lerp(v1.color, v2.color, t) };
				_put_point(bitmap, vertex);
			}
		}
	}

	void Renderer::_put_triangle(Bitmap* /*bitmap*/, Vertex /*v1*/, Vertex /*v2*/, Vertex /*v3*/) {
	}

} // namespace engine
