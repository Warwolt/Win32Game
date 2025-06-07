#include <engine/graphics/renderer_OLD.h>

#include <engine/debug/logging.h>
#include <engine/math/ivec2.h>
#include <engine/math/vec2.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <windows.h>

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

	void Renderer_OLD::clear_screen(RGBA color) {
		m_draw_commands.push_back(ClearScreen { color });
	}

	void Renderer_OLD::draw_point(IVec2 point, RGBA color) {
		m_draw_commands.push_back(DrawPoint { point, color });
	}

	void Renderer_OLD::draw_line(Vertex start, Vertex end) {
		m_draw_commands.push_back(DrawLine { start, end });
	}

	void Renderer_OLD::draw_rect(Rect rect, RGBA color) {
		m_draw_commands.push_back(DrawRect { rect, color, false });
	}

	void Renderer_OLD::draw_rect_fill(Rect rect, RGBA color) {
		m_draw_commands.push_back(DrawRect { rect, color, true });
	}

	void Renderer_OLD::draw_polygon(std::vector<IVec2> vertices, RGBA color) {
		if (vertices.size() < 3) {
			LOG_WARNING("draw_polygon called with less than 3 vertices, ignoring");
			return;
		}
		m_draw_commands.push_back(DrawPolygon { std::move(vertices), color, false });
	}

	void Renderer_OLD::draw_polygon_fill(std::vector<IVec2> vertices, RGBA color) {
		if (vertices.size() < 3) {
			LOG_WARNING("draw_polygon called with less than 3 vertices, ignoring");
			return;
		}
		m_draw_commands.push_back(DrawPolygon { std::move(vertices), color, true });
	}

	void Renderer_OLD::draw_circle(IVec2 center, int32_t radius, RGBA color) {
		m_draw_commands.push_back(DrawCircle { center, radius, color, false });
	}

	void Renderer_OLD::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
		m_draw_commands.push_back(DrawCircle { center, radius, color, true });
	}

	void Renderer_OLD::render(Bitmap* bitmap) {
		/* Draw to bitmap */
		for (DrawCommand& command : m_draw_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				_clear_screen(bitmap, clear_screen->color);
			}
			if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
				_put_point(bitmap, draw_point->point, draw_point->color);
			}
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				_put_line(bitmap, draw_line->start, draw_line->end);
			}
			if (auto* draw_rect = std::get_if<DrawRect>(&command)) {
				if (draw_rect->filled) {
					_put_rect_fill(bitmap, draw_rect->rect, draw_rect->color);
				}
				else {
					_put_rect(bitmap, draw_rect->rect, draw_rect->color);
				}
			}
			if (auto* draw_polygon = std::get_if<DrawPolygon>(&command)) {
				if (draw_polygon->filled) {
					_put_polygon_fill(bitmap, draw_polygon->vertices, draw_polygon->color);
				}
				else {
					_put_polygon(bitmap, draw_polygon->vertices, draw_polygon->color);
				}
			}
			if (auto* draw_circle = std::get_if<DrawCircle>(&command)) {
				if (draw_circle->filled) {
					_put_circle_fill(bitmap, draw_circle->center, draw_circle->radius, draw_circle->color);
				}
				else {
					_put_circle(bitmap, draw_circle->center, draw_circle->radius, draw_circle->color);
				}
			}
		}
		m_draw_commands.clear();
	}

	void Renderer_OLD::_clear_screen(Bitmap* bitmap, RGBA color) {
		for (size_t i = 0; i < bitmap->width * bitmap->height; i++) {
			bitmap->data[i] = Pixel::from_rgba(color);
		}
	}

	void Renderer_OLD::_put_point(Bitmap* bitmap, IVec2 point, RGBA color) {
		Pixel old_color = bitmap->get(point.x, point.y);
		Pixel new_color = Pixel {
			.b = (uint8_t)std::lerp(old_color.b, color.b, color.a / 255.0f),
			.g = (uint8_t)std::lerp(old_color.g, color.g, color.a / 255.0f),
			.r = (uint8_t)std::lerp(old_color.r, color.r, color.a / 255.0f),
		};
		bitmap->put(point.x, point.y, new_color);
	}

	void Renderer_OLD::_put_line_OLD(Bitmap* bitmap, IVec2 start, IVec2 end, RGBA color) {
		_put_line(bitmap, Vertex { start, color }, Vertex { end, color });
	}

	void Renderer_OLD::_put_line(Bitmap* bitmap, Vertex start, Vertex end) {
		// vertical line
		if (start.pos.x == end.pos.x) {
			int32_t y0 = min(start.pos.y, end.pos.y);
			int32_t y1 = max(start.pos.y, end.pos.y);
			for (int32_t y = y0; y <= y1; y++) {
				IVec2 pos = IVec2 { start.pos.x, y };
				float t = (float)(pos.y - start.pos.y) / (float)(end.pos.y - start.pos.y);
				_put_point(bitmap, pos, RGBA::lerp(start.color, end.color, t));
			}
		}
		// sloped line
		else {
			// delta is the longer side of the triangle formed by the line
			// if dx is greater, x_step will be +1 or -1 and y_step will be the slope
			// if dy is greater, we flip it along the diagonal
			int32_t dx = end.pos.x - start.pos.x;
			int32_t dy = end.pos.y - start.pos.y;
			int32_t abs_dx = std::abs(dx);
			int32_t abs_dy = std::abs(dy);
			int32_t delta = max(abs_dx, abs_dy);
			float x_step = (float)dx / (float)delta;
			float y_step = (float)dy / (float)delta;
			for (int32_t i = 0; i <= delta; i++) {
				IVec2 pos = IVec2 { .x = (int32_t)(start.pos.x + i * x_step), .y = (int32_t)(start.pos.y + i * y_step) };
				float t = abs_dx > abs_dy
					? (float)(pos.x - start.pos.x) / (float)(end.pos.x - start.pos.x)
					: (float)(pos.y - start.pos.y) / (float)(end.pos.y - start.pos.y);
				_put_point(bitmap, pos, RGBA::lerp(start.color, end.color, t));
			}
		}
	}

	void Renderer_OLD::_put_rect(Bitmap* bitmap, Rect rect, RGBA color) {
		IVec2 top_left = { rect.x, rect.y };
		IVec2 top_right = { rect.x + rect.width - 1, rect.y };
		IVec2 bottom_left = { rect.x, rect.y + rect.height - 1 };
		IVec2 bottom_right = { rect.x + rect.width - 1, rect.y + rect.height - 1 };
		_put_line_OLD(bitmap, top_left, bottom_left, color);
		_put_line_OLD(bitmap, top_right, bottom_right, color);
		_put_line_OLD(bitmap, top_left, top_right, color);
		_put_line_OLD(bitmap, bottom_left, bottom_right, color);
	}

	void Renderer_OLD::_put_rect_fill(Bitmap* bitmap, Rect rect, RGBA color) {
		int32_t left = rect.x;
		int32_t right = rect.x + rect.width - 1;
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			_put_line_OLD(bitmap, IVec2 { left, y }, IVec2 { right, y }, color);
		}
	}

	void Renderer_OLD::_put_polygon(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color) {
		for (size_t i = 0; i < vertices.size() - 1; i++) {
			_put_line_OLD(bitmap, vertices[i], vertices[i + 1], color);
		}
		_put_line_OLD(bitmap, vertices[vertices.size() - 1], vertices[0], color);
	}

	void Renderer_OLD::_put_polygon_fill(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color) {
		struct PolygonEdge {
			int32_t x0;
			int32_t y0;
			int32_t y1;
			float inv_slope;
		};

		struct PolygonIntersection {
			int32_t x;
			bool is_maximum;
			bool is_minimum;
		};

		/* Check vertices make a polygon */
		if (vertices.size() < 3) {
			return;
		}

		/* Compute edges and bounding box */
		std::vector<PolygonEdge> edges;
		int32_t min_y = min(vertices[0].y, vertices[1].y);
		int32_t max_y = max(vertices[0].y, vertices[1].y);
		for (size_t i = 0; i < vertices.size(); i++) {
			IVec2 first = vertices[i];
			IVec2 second = vertices[(i + 1) % vertices.size()];
			min_y = min(min_y, min(first.y, second.y));
			max_y = max(max_y, max(first.y, second.y));

			// Skip horizontal edges since we're filling in the polygon row by
			// row, and want to avoid edges that overlap with the scanline.
			bool is_horizontal = first.y == second.y;
			if (!is_horizontal) {
				float inv_slope = (float)(second.x - first.x) / (float)(second.y - first.y);
				edges.push_back(PolygonEdge {
					.x0 = first.x,
					.y0 = first.y,
					.y1 = second.y,
					.inv_slope = inv_slope,
				});
			}
		}

		/* Scan y from top to bottom, filling in polygon row by row */
		for (int32_t y = min_y; y <= max_y; y++) {
			/* Find points of intersection */
			std::vector<PolygonIntersection> intersections;
			for (const PolygonEdge& edge : edges) {
				int32_t y_min = min(edge.y0, edge.y1);
				int32_t y_max = max(edge.y0, edge.y1);
				if (y_min <= y && y <= y_max) {
					int32_t x = (int32_t)std::round(edge.inv_slope * (y - edge.y0) + edge.x0);
					intersections.push_back(PolygonIntersection {
						.x = x,
						.is_maximum = y == y_max,
						.is_minimum = y == y_min,
					});
				}
			}

			/* Sort by x-values to collect equal points */
			auto intersection_less_than = [](PolygonIntersection lhs, PolygonIntersection rhs) { return lhs.x < rhs.x; };
			std::sort(intersections.begin(), intersections.end(), intersection_less_than);

			/* Determine which points to draw */
			std::vector<int32_t> points_to_connect;
			std::vector<int32_t> corner_and_cross_points;
			int32_t current_x = intersections.front().x;
			while (true) {
				/* Scan through all points with same x-value */
				auto equals_current_x = [current_x](const PolygonIntersection& intersection) { return intersection.x == current_x; };
				auto not_equals_current_x = [current_x](const PolygonIntersection& intersection) { return intersection.x != current_x; };
				auto begin = std::find_if(intersections.begin(), intersections.end(), equals_current_x);
				auto end = std::find_if(begin, intersections.end(), not_equals_current_x);

				/* Determine what kind of point (should we draw it as a point or a line?) */
				int num_maximum = 0;
				int num_minimum = 0;
				int num_intersections = 0;
				for (auto it = begin; it != end; it++) {
					num_maximum += it->is_maximum ? 1 : 0;
					num_minimum += it->is_minimum ? 1 : 0;
					num_intersections += 1;
				}
				// KNOWN BUG: If intersection contains both a corner and middle
				// of edge, then we'll end up not drawing the correct fill.
				// Reproduce with: [(-2,0), (0,-1), (2,-2), (2,1), (0,-1), (0, 2)]
				// (Will draw a set of "teeth", with missing pixels right of (0,-1))
				bool no_overlapping_vertices = num_maximum == 0 && num_minimum == 0;
				bool odd_number_of_intersections = num_intersections % 2 == 1;
				bool is_corner = num_maximum >= 2 || num_minimum >= 2;
				bool is_cross = (no_overlapping_vertices || odd_number_of_intersections) && num_intersections > 1;

				/* Save point to draw later */
				if (is_corner || is_cross) {
					corner_and_cross_points.push_back(current_x);
				}
				else {
					points_to_connect.push_back(current_x);
				}

				/* Iterate */
				if (end == intersections.end()) {
					break;
				}
				current_x = end->x;
			}

			/* Draw all points */
			for (size_t i = 0; i + 1 < points_to_connect.size(); i += 2) {
				IVec2 start = { points_to_connect[i], y };
				IVec2 end = { points_to_connect[i + 1], y };
				_put_line_OLD(bitmap, start, end, color);
			}
			// HACK: Skip drawing points if drawing lines so we don't overdraw,
			// since this will mess with alpha blending.
			if (points_to_connect.empty()) {
				for (int32_t x : corner_and_cross_points) {
					_put_point(bitmap, IVec2 { x, y }, color);
				}
			}
		}
	}

	void Renderer_OLD::_put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color) {
		std::vector<IVec2> octant_points = circle_octant_points(radius);
		for (IVec2 point : octant_points) {
			_put_point(bitmap, center + IVec2 { point.x, point.y }, color);
			_put_point(bitmap, center + IVec2 { point.y, point.x }, color);
			_put_point(bitmap, center + IVec2 { point.y, -point.x }, color);
			_put_point(bitmap, center + IVec2 { point.x, -point.y }, color);
			_put_point(bitmap, center + IVec2 { -point.x, point.y }, color);
			_put_point(bitmap, center + IVec2 { -point.y, point.x }, color);
			_put_point(bitmap, center + IVec2 { -point.y, -point.x }, color);
			_put_point(bitmap, center + IVec2 { -point.x, -point.y }, color);
		}
	}

	void Renderer_OLD::_put_circle_fill(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color) {
		/* Compute points for upper half circle */
		std::vector<IVec2> octant_points = circle_octant_points(radius);
		std::vector<IVec2> half_circle_points;
		for (IVec2 point : octant_points) {
			half_circle_points.push_back({ point.x, point.y });
			half_circle_points.push_back({ point.y, point.x });
			half_circle_points.push_back({ -point.x, point.y });
			half_circle_points.push_back({ -point.y, point.x });
		}

		/* Remove overlapping x-coordinates to avoid overdraw */
		auto x_less_than = [](IVec2 lhs, IVec2 rhs) { return lhs.x < rhs.x; };
		auto xs_equal = [](IVec2 lhs, IVec2 rhs) { return lhs.x == rhs.x; };
		std::sort(half_circle_points.begin(), half_circle_points.end(), x_less_than);
		half_circle_points.erase(std::unique(half_circle_points.begin(), half_circle_points.end(), xs_equal), half_circle_points.end());

		/* Draw vertical lines */
		for (IVec2 point : half_circle_points) {
			_put_line_OLD(bitmap, center + IVec2 { point.x, point.y }, center + IVec2 { point.x, -point.y }, color);
		}
	}

} // namespace engine
