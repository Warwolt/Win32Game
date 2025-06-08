#include <engine/graphics/renderer.h>

#include <algorithm>
#include <cmath>

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
		m_commands.push_back(ClearScreen { color });
	}

	void Renderer::draw_point(Vertex v1) {
		m_commands.push_back(DrawPoint { v1 });
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
		m_commands.push_back(DrawLine { v1, v2 });
	}

	void Renderer::draw_rect(Rect rect, RGBA color) {
		IVec2 top_left = { rect.x, rect.y };
		IVec2 top_right = { rect.x + rect.width - 1, rect.y };
		IVec2 bottom_left = { rect.x, rect.y + rect.height - 1 };
		IVec2 bottom_right = { rect.x + rect.width - 1, rect.y + rect.height - 1 };
		draw_line(Vertex { .pos = top_left, .color = color }, Vertex { .pos = bottom_left, .color = color });
		draw_line(Vertex { .pos = top_right, .color = color }, Vertex { .pos = bottom_right, .color = color });
		draw_line(Vertex { .pos = top_left + IVec2 { 1, 0 }, .color = color }, Vertex { .pos = top_right - IVec2 { 1, 0 }, .color = color });
		draw_line(Vertex { .pos = bottom_left + IVec2 { 1, 0 }, .color = color }, Vertex { .pos = bottom_right - IVec2 { 1, 0 }, .color = color });
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			Vertex left = Vertex { .pos = { rect.x, y }, .color = color };
			Vertex right = Vertex { .pos = { rect.x + rect.width - 1, y }, .color = color };
			draw_line(left, right);
		}
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
		int32_t diameter = 2 * radius;
		std::vector<bool> circle_points(diameter * diameter);

		// project octant points to all 8 octants
		for (IVec2 point : circle_octant_points(radius)) {
			circle_points[point.x + point.y * diameter] = true;

			// circle_points.push_back(center + IVec2 { point.x, point.y });
			// circle_points.push_back(center + IVec2 { point.y, point.x });
			// circle_points.push_back(center + IVec2 { point.y, -point.x });
			// circle_points.push_back(center + IVec2 { point.x, -point.y });
			// circle_points.push_back(center + IVec2 { -point.x, point.y });
			// circle_points.push_back(center + IVec2 { -point.y, point.x });
			// circle_points.push_back(center + IVec2 { -point.y, -point.x });
			// circle_points.push_back(center + IVec2 { -point.x, -point.y });
		}

		for (int32_t y = 0; y < diameter; y++) {
			for (int32_t x = 0; x < diameter; x++) {
				if (circle_points[x + y * diameter]) {
					draw_point(Vertex { .pos = center + IVec2 { x, y }, .color = color });
				}
			}
		}
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
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				_put_line(bitmap, draw_line->v1, draw_line->v2);
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
		constexpr bool debug_overdraw = true;
		if (debug_overdraw) {
			Pixel current_pixel = bitmap->get(v1.pos.x, v1.pos.y);
			Pixel our_pixel = Pixel::from_rgba(v1.color);
			Pixel new_pixel = current_pixel == our_pixel ? Pixel { 255, 0, 255 } : our_pixel;
			bitmap->put(v1.pos.x, v1.pos.y, new_pixel);
		}
		else {
			Pixel current_pixel = bitmap->get(v1.pos.x, v1.pos.y);
			Pixel new_pixel = current_pixel.lerp(Pixel::from_rgba(v1.color), v1.color.a / 255.0f);
			bitmap->put(v1.pos.x, v1.pos.y, new_pixel);
		}
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2) {
		// vertical line
		if (v1.pos.x == v2.pos.x) {
			int32_t y0 = std::min(v1.pos.y, v2.pos.y);
			int32_t y1 = std::max(v1.pos.y, v2.pos.y);
			for (int32_t y = y0; y <= y1; y++) {
				IVec2 pos = IVec2 { v1.pos.x, y };
				float t = (float)(pos.y - v1.pos.y) / (float)(v2.pos.y - v1.pos.y);
				_put_point(bitmap, Vertex { .pos = pos, .color = RGBA::lerp(v1.color, v2.color, t) });
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
				_put_point(bitmap, Vertex { .pos = pos, .color = RGBA::lerp(v1.color, v2.color, t) });
			}
		}
	}

} // namespace engine
