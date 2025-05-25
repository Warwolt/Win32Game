#include <engine/graphics/renderer.h>

#include <engine/debug/logging.h>

#include <algorithm>
#include <cmath>
#include <iterator>
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

	void Renderer::draw_rect(Rect rect, Color color) {
		m_draw_commands.push_back(DrawRect { rect, color, false });
	}

	void Renderer::draw_rect_fill(Rect rect, Color color) {
		m_draw_commands.push_back(DrawRect { rect, color, true });
	}

	void Renderer::draw_polygon(const std::vector<IVec2> vertices, Color color) {
		if (vertices.size() < 3) {
			LOG_WARNING("draw_polygon called with less than 3 vertices, ignoring");
			return;
		}
		m_draw_commands.push_back(DrawPolygon { vertices, color, false });
	}

	void Renderer::draw_polygon_fill(const std::vector<IVec2> vertices, Color color) {
		if (vertices.size() < 3) {
			LOG_WARNING("draw_polygon called with less than 3 vertices, ignoring");
			return;
		}
		m_draw_commands.push_back(DrawPolygon { vertices, color, true });
	}

	void Renderer::render(engine::Window* window, HDC device_context) {
		/* Draw to bitmap */
		for (DrawCommand& command : m_draw_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				_clear_screen(&window->bitmap);
			}
			if (auto* draw_pixel = std::get_if<DrawPixel>(&command)) {
				_put_pixel(&window->bitmap, draw_pixel->x, draw_pixel->y, draw_pixel->color);
			}
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				_put_line(&window->bitmap, draw_line->start, draw_line->end, draw_line->color);
			}
			if (auto* draw_rect = std::get_if<DrawRect>(&command)) {
				if (draw_rect->filled) {
					_put_rect_fill(&window->bitmap, draw_rect->rect, draw_rect->color);
				}
				else {
					_put_rect(&window->bitmap, draw_rect->rect, draw_rect->color);
				}
			}
			if (auto* draw_polygon = std::get_if<DrawPolygon>(&command)) {
				if (draw_polygon->filled) {
					_put_polygon_fill(&window->bitmap, draw_polygon->vertices, draw_polygon->color);
				}
				else {
					_put_polygon(&window->bitmap, draw_polygon->vertices, draw_polygon->color);
				}
			}
		}
		m_draw_commands.clear();

		/* Blit bitmap to window */
		{
			RECT client_rect;
			GetClientRect(window->handle, &client_rect);
			int window_width = client_rect.right - client_rect.left;
			int window_height = client_rect.bottom - client_rect.top;

			StretchDIBits(
				device_context,
				// destination rect (window)
				0,
				0,
				window_width,
				window_height,
				// source rect (bitmap)
				0,
				0,
				window->bitmap.width,
				window->bitmap.height,
				// bitmap data
				window->bitmap.data,
				&window->bitmap_info,
				DIB_RGB_COLORS,
				SRCCOPY
			);
		}
	}

	void Renderer::_clear_screen(Bitmap* bitmap) {
		ZeroMemory(bitmap->data, bitmap->width * bitmap->height * sizeof(int32_t));
	}

	void Renderer::_put_pixel(Bitmap* bitmap, int32_t x, int32_t y, Color color) {
		bitmap->put(x, y, BGRPixel { .b = color.b, .g = color.g, .r = color.r });
	}

	void Renderer::_put_line(Bitmap* bitmap, IVec2 start, IVec2 end, Color color) {
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

	void Renderer::_put_rect(Bitmap* bitmap, Rect rect, Color color) {
		IVec2 top_left = { rect.x, rect.y };
		IVec2 top_right = { rect.x + rect.width - 1, rect.y };
		IVec2 bottom_left = { rect.x, rect.y + rect.height - 1 };
		IVec2 bottom_right = { rect.x + rect.width - 1, rect.y + rect.height - 1 };
		_put_line(bitmap, top_left, bottom_left, color);
		_put_line(bitmap, top_right, bottom_right, color);
		_put_line(bitmap, top_left, top_right, color);
		_put_line(bitmap, bottom_left, bottom_right, color);
	}

	void Renderer::_put_rect_fill(Bitmap* bitmap, Rect rect, Color color) {
		int32_t left = rect.x;
		int32_t right = rect.x + rect.width - 1;
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			_put_line(bitmap, IVec2 { left, y }, IVec2 { right, y }, color);
		}
	}

	void Renderer::_put_polygon(Bitmap* bitmap, const std::vector<IVec2>& vertices, Color color) {
		for (size_t i = 0; i < vertices.size() - 1; i++) {
			_put_line(bitmap, vertices[i], vertices[i + 1], color);
		}
		_put_line(bitmap, vertices[vertices.size() - 1], vertices[0], color);
	}

	void Renderer::_put_polygon_fill(Bitmap* bitmap, const std::vector<IVec2>& vertices, Color color) {
		/* Check vertices make a polygon */
		if (vertices.size() < 3) {
			return;
		}

		struct Edge {
			int32_t x0;
			int32_t y0;
			int32_t y1;
			float inv_slope;
		};

		std::vector<Edge> edges;
		int32_t min_y = min(vertices[0].x, vertices[1].x);
		int32_t max_y = max(vertices[0].x, vertices[1].x);

		/* Compute edges and bounding box */
		for (size_t i = 0; i < vertices.size(); i++) {
			IVec2 first = vertices[i];
			IVec2 second = vertices[(i + 1) % vertices.size()];
			min_y = min(min_y, min(first.y, second.y));
			max_y = max(max_y, max(first.y, second.y));

			bool is_horizontal = first.y == second.y;
			if (!is_horizontal) {
				float inv_slope = (float)(second.x - first.x) / (float)(second.y - first.y);
				edges.push_back(Edge {
					.x0 = first.x,
					.y0 = first.y,
					.y1 = second.y,
					.inv_slope = inv_slope,
				});
			}
		}

		/* Scan y from top to bottom, filling in polygon row by row */
		for (int32_t y = min_y; y <= max_y; y++) {
			/* Compute points of edges intersecting current scan line */
			std::vector<int32_t> xs;
			for (const Edge& edge : edges) {
				int32_t y_min = min(edge.y0, edge.y1);
				int32_t y_max = max(edge.y0, edge.y1);
				if (y_min <= y && y <= y_max) {
					int32_t x = (int32_t)std::round(edge.inv_slope * (y - edge.y0) + edge.x0);
					xs.push_back(x);
				}
			}
			std::sort(xs.begin(), xs.end());

			/* Draw lines between intersection points */
			for (size_t i = 0; i + 1 < xs.size();) {
				/* Skip past duplicate x-coordinates */
				// This usually takes care of edges of triangles,
				// or when two edges cross each other and form an X.
				if (xs[i] == xs[i + 1]) {
					i += 1;
					continue;
				}

				/* Fill in polygon slices at current y */
				IVec2 start = { xs[i], y };
				IVec2 end = { xs[i + 1], y };
				_put_line(bitmap, start, end, color);
				i += 2;
			}
		}
	}

} // namespace engine
