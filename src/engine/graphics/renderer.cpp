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
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
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
		for (const CommandBatch& batch : m_batches) {
            /* Draw onto scratch pad */
			for (const DrawCommand& command : batch.commands) {
				if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
					_clear_screen(bitmap, clear_screen->color);
				}
				if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
					_put_point(bitmap, draw_point->v1);
				}
				if (auto* draw_line = std::get_if<DrawLine>(&command)) {
					_put_line(bitmap, draw_line->v1, draw_line->v2);
				}
				if (auto* draw_triangle = std::get_if<DrawTriangle>(&command)) {
					_put_triangle(bitmap, draw_triangle->v1, draw_triangle->v2, draw_triangle->v3);
				}
			}

            /* Draw scratch pad onto bitmap */
            // TODO
		}
		m_batches.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		for (int32_t y = 0; y < bitmap->width; y++) {
			for (int32_t x = 0; x < bitmap->width; x++) {
				bitmap->put(x, y, Pixel::from_rgba(color));
			}
		}
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1) {
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2) {
	}

	void Renderer::_put_triangle(Bitmap* /*bitmap*/, Vertex /*v1*/, Vertex /*v2*/, Vertex /*v3*/) {
		// TODO
	}

} // namespace engine
