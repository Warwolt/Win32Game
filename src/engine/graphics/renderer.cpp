#include <engine/graphics/renderer.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/font.h>
#include <engine/graphics/image.h>
#include <engine/math/math.h>

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

	static std::vector<IVec2> half_circle_points(int32_t radius) {
		/* Compute half circle*/
		//         , - ~ ~ ~ - ,
		//     , '               ' ,
		//   ,                       ,
		//  ,                         ,
		// ,                           ,
		// '-------------o-------------'
		std::vector<IVec2> octant_points = circle_octant_points(radius);
		std::vector<IVec2> half_circle_points;
		IVec2 prev_point = IVec2 { octant_points[0].x, octant_points[0].y };
		for (IVec2 point : octant_points) {
			int32_t delta_y = point.y - prev_point.y;
			if (delta_y != 0) {
				half_circle_points.push_back(point);
			}
			prev_point = point;
		}

		prev_point = IVec2 { octant_points[0].y, octant_points[0].x };
		half_circle_points.push_back(prev_point);
		for (IVec2 point : octant_points) {
			IVec2 flip_point = { point.y, point.x };
			int32_t delta_y = flip_point.y - prev_point.y;
			if (delta_y != 0) {
				half_circle_points.push_back(flip_point);
			}
			prev_point = flip_point;
		}
		return half_circle_points;
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
		m_commands.push_back(DrawRect { rect, color, false });
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
		m_commands.push_back(DrawRect { rect, color, true });
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
		m_commands.push_back(DrawCircle { center, radius, color, false });
	}

	void Renderer::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
		m_commands.push_back(DrawCircle { center, radius, color, true });
	}

	void Renderer::draw_triangle(Vertex v1, Vertex v2, Vertex v3) {
		m_commands.push_back(DrawTriangle { v1, v2, v3, false });
	}

	void Renderer::draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3) {
		m_commands.push_back(DrawTriangle { v1, v2, v3, true });
	}

	void Renderer::draw_image(ImageID image_id, Rect rect, Rect clip, RGBA tint) {
		m_commands.push_back(DrawImage { image_id, rect, clip, tint });
	}

	void Renderer::draw_text(FontID font_id, int32_t font_size, IVec2 pos, RGBA color, std::string text) {
		m_commands.push_back(DrawText { font_id, font_size, pos, color, text });
	}

	void Renderer::render(Bitmap* bitmap, ResourceManager* resources) {
		/* Run commands */
		for (const DrawCommand& command : m_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				auto& [color] = *clear_screen;
				_clear_screen(bitmap, color);
			}
			if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
				auto& [v1] = *draw_point;
				_put_point(bitmap, v1);
			}
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				auto& [v1, v2] = *draw_line;
				_put_line(bitmap, v1, v2, nullptr);
			}
			if (auto* draw_rect = std::get_if<DrawRect>(&command)) {
				auto& [rect, color, filled] = *draw_rect;
				if (filled) {
					_put_rect_fill(bitmap, rect, color);
				}
				else {
					_put_rect(bitmap, rect, color);
				}
			}
			if (auto* draw_circle = std::get_if<DrawCircle>(&command)) {
				auto& [center, radius, color, filled] = *draw_circle;
				if (filled) {
					_put_circle_fill(bitmap, center, radius, color);
				}
				else {
					_put_circle(bitmap, center, radius, color);
				}
			}
			if (auto* draw_triangle = std::get_if<DrawTriangle>(&command)) {
				auto& [v1, v2, v3, filled] = *draw_triangle;
				if (filled) {
					_put_triangle_fill(bitmap, v1, v2, v3);
				}
				else {
					_put_triangle(bitmap, v1, v2, v3);
				}
			}
			if (auto* draw_image = std::get_if<DrawImage>(&command)) {
				auto& [image_id, rect, clip, tint] = *draw_image;
				_put_image(bitmap, resources->image(image_id), rect, clip, tint);
			}
			if (auto* draw_text = std::get_if<DrawText>(&command)) {
				auto& [font_id, font_size, pos, color, text] = *draw_text;
				Typeface& typeface = resources->font(font_id);
				_put_text(bitmap, &typeface, font_size, pos, color, text);
			}
		}
		m_commands.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		bitmap->clear(Pixel::from_rgb(color));
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1) {
		Pixel pixel = { .b = v1.color.b, .g = v1.color.g, .r = v1.color.r, .padding = v1.color.a };
		bitmap->put(v1.pos.x, v1.pos.y, pixel, v1.color.a / 255.0f);
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2, const Image* image) {
		// Bresenham's drawing algorithm
		// Alois Zingl, 2016, "A Rasterizing Algorithm for Drawing Curves", page 13
		// https://zingl.github.io/Bresenham.pdf
		int32_t delta_x = std::abs(v2.pos.x - v1.pos.x);
		int32_t delta_y = -std::abs(v2.pos.y - v1.pos.y);
		int32_t sign_x = v1.pos.x < v2.pos.x ? 1 : -1;
		int32_t sign_y = v1.pos.y < v2.pos.y ? 1 : -1;
		int32_t error = delta_x + delta_y;

		Vertex cursor = v1;
		while (true) {
			/* Put current point */
			float t = delta_x > 0
				? ((float)(cursor.pos.x - v1.pos.x) / (float)(v2.pos.x - v1.pos.x))
				: ((float)(cursor.pos.y - v1.pos.y) / (float)(v2.pos.y - v1.pos.y));
			if (image) {
				cursor.color = image->sample(Vec2::lerp(v1.uv, v2.uv, t)) * RGBA::lerp(v1.color, v2.color, t);
			}
			else {
				cursor.color = RGBA::lerp(v1.color, v2.color, t);
			}
			bitmap->put(cursor.pos.x, cursor.pos.y, Pixel::from_rgb(cursor.color), cursor.color.a / 255.0f);

			/* Step to next point */
			if (2 * error >= delta_y) {
				if (cursor.pos.x == v2.pos.x) {
					break;
				}
				error += delta_y;
				cursor.pos.x += sign_x;
			}
			if (2 * error <= delta_x) {
				if (cursor.pos.y == v2.pos.y) {
					break;
				}
				error += delta_x;
				cursor.pos.y += sign_y;
			}
		}
	}

	void Renderer::_put_rect(Bitmap* bitmap, Rect rect, RGBA color) {
		IVec2 top_left = { rect.x, rect.y };
		IVec2 top_right = { rect.x + rect.width - 1, rect.y };
		IVec2 bottom_left = { rect.x, rect.y + rect.height - 1 };
		IVec2 bottom_right = { rect.x + rect.width - 1, rect.y + rect.height - 1 };

		Vertex top_start = { .pos = top_left, .color = color };
		Vertex top_end = { .pos = top_right, .color = color };
		Vertex bottom_start = { .pos = bottom_left, .color = color };
		Vertex bottom_end = { .pos = bottom_right, .color = color };
		Vertex left_start = { .pos = top_left + IVec2 { 0, 1 }, .color = color };
		Vertex left_end = { .pos = bottom_left - IVec2 { 0, 1 }, .color = color };
		Vertex right_start = { .pos = top_right + IVec2 { 0, 1 }, .color = color };
		Vertex right_end = { .pos = bottom_right - IVec2 { 0, 1 }, .color = color };

		_put_line(bitmap, top_start, top_end, nullptr);
		_put_line(bitmap, bottom_start, bottom_end, nullptr);
		_put_line(bitmap, left_start, left_end, nullptr);
		_put_line(bitmap, right_start, right_end, nullptr);
	}

	void Renderer::_put_rect_fill(Bitmap* bitmap, Rect rect, RGBA color) {
		Pixel pixel = Pixel::from_rgb(color);
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			for (int32_t x = rect.x; x < rect.x + rect.width; x++) {
				bitmap->put(x, y, pixel, color.a / 255.0f);
			}
		}
	}

	void Renderer::_put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color) {
		Pixel pixel = Pixel::from_rgb(color);
		float alpha = color.a / 255.0f;
		for (IVec2 point : circle_octant_points(radius)) {
			bitmap->put(center.x + point.x, center.y + point.y, pixel, alpha);
			bitmap->put(center.x + point.y, center.y + point.x, pixel, alpha);
			bitmap->put(center.x + point.y, center.y + -point.x, pixel, alpha);
			bitmap->put(center.x + point.x, center.y + -point.y, pixel, alpha);
			bitmap->put(center.x + -point.x, center.y + point.y, pixel, alpha);
			bitmap->put(center.x + -point.y, center.y + point.x, pixel, alpha);
			bitmap->put(center.x + -point.y, center.y + -point.x, pixel, alpha);
			bitmap->put(center.x + -point.x, center.y + -point.y, pixel, alpha);
		}
	}

	void Renderer::_put_circle_fill(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color) {
		for (IVec2 point : half_circle_points(radius)) {
			IVec2 bottom_left = center + IVec2 { -point.x, point.y };
			IVec2 bottom_right = center + IVec2 { point.x, point.y };
			_put_line(bitmap, Vertex { .pos = bottom_left, .color = color }, Vertex { .pos = bottom_right, .color = color }, nullptr);

			IVec2 top_left = center + IVec2 { -point.x, -point.y };
			IVec2 top_right = center + IVec2 { point.x, -point.y };
			if (top_left.y != bottom_left.y) {
				_put_line(bitmap, Vertex { .pos = top_left, .color = color }, Vertex { .pos = top_right, .color = color }, nullptr);
			}
		}
	}

	void Renderer::_put_triangle(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3) {
		_put_line(bitmap, v1, v2, nullptr);
		_put_line(bitmap, v1, v3, nullptr);
		_put_line(bitmap, v2, v3, nullptr);
	}

	void Renderer::_put_triangle_fill(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3) {
		auto triangle_area = [](IVec2 a, IVec2 b) -> float {
			return std::abs((float)(a.x * b.y - a.y * b.x) / 2.0f);
		};

		int32_t x_min = std::min({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t x_max = std::max({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t y_min = std::min({ v1.pos.y, v2.pos.y, v3.pos.y });
		int32_t y_max = std::max({ v1.pos.y, v2.pos.y, v3.pos.y });

		float inv_slope[3] = {
			(float)(v2.pos.x - v1.pos.x) / (float)(v2.pos.y - v1.pos.y),
			(float)(v3.pos.x - v1.pos.x) / (float)(v3.pos.y - v1.pos.y),
			(float)(v3.pos.x - v2.pos.x) / (float)(v3.pos.y - v2.pos.y),
		};

		float area_v1v2v3 = triangle_area(v2.pos - v1.pos, v3.pos - v1.pos);
		for (int32_t y = y_min; y <= y_max; y++) {
			/* Get intersections betwen scanline and lines extended from triangle sides */
			int32_t xs[3] = {
				(int32_t)std::round((y - v1.pos.y) * inv_slope[0] + v1.pos.x),
				(int32_t)std::round((y - v1.pos.y) * inv_slope[1] + v1.pos.x),
				(int32_t)std::round((y - v2.pos.y) * inv_slope[2] + v2.pos.x),
			};

			/* Discard line intersections outside of triangle */
			if (xs[0] < x_min || xs[0] > x_max) {
				std::swap(xs[0], xs[2]);
			}
			else if (xs[1] < x_min || xs[1] > x_max) {
				std::swap(xs[1], xs[2]);
			}
			Vertex p1 = { .pos = { (int32_t)xs[0], y } };
			Vertex p2 = { .pos = { (int32_t)xs[1], y } };

			/* Interpolate colors */
			float area_v1v2p1 = triangle_area(v2.pos - v1.pos, p1.pos - v1.pos);
			float area_v1v3p1 = triangle_area(v3.pos - v1.pos, p1.pos - v1.pos);
			float area_v2v3p1 = triangle_area(v3.pos - v2.pos, p1.pos - v2.pos);
			float v1_t1 = area_v2v3p1 / area_v1v2v3;
			float v2_t1 = area_v1v3p1 / area_v1v2v3;
			float v3_t1 = area_v1v2p1 / area_v1v2v3;
			p1.color = v1_t1 * v1.color + v2_t1 * v2.color + v3_t1 * v3.color;

			float area_v1v2p2 = triangle_area(v2.pos - v1.pos, p2.pos - v1.pos);
			float area_v1v3p2 = triangle_area(v3.pos - v1.pos, p2.pos - v1.pos);
			float area_v2v3p2 = triangle_area(v3.pos - v2.pos, p2.pos - v2.pos);
			float v1_t2 = area_v2v3p2 / area_v1v2v3;
			float v2_t2 = area_v1v3p2 / area_v1v2v3;
			float v3_t2 = area_v1v2p2 / area_v1v2v3;
			p2.color = v1_t2 * v1.color + v2_t2 * v2.color + v3_t2 * v3.color;

			/* Draw line */
			_put_line(bitmap, p1, p2, nullptr);
		}
	}

	void Renderer::_put_image(Bitmap* bitmap, const Image& image, Rect rect, Rect clip, RGBA tint) {
		/* UV coordinates */
		if (clip.empty()) {
			clip.width = rect.width;
			clip.height = rect.height;
		}
		// bottom left
		Vec2 uv0 = {
			.x = engine::clamp((float)clip.x / (float)(rect.width - 1), 0.0f, 1.0f),
			.y = engine::clamp((float)clip.y / (float)(rect.height), 0.0f, 1.0f),
		};
		// top right
		Vec2 uv1 = {
			.x = engine::clamp((float)(clip.x + clip.width - 1) / (float)(rect.width - 1), 0.0f, 1.0f),
			.y = engine::clamp((float)(clip.y + clip.height) / (float)(rect.height), 0.0f, 1.0f),
		};

		/* Draw image line by line */
		for (int32_t y = 0; y < rect.height; y++) {
			Vertex left = {
				.pos = { rect.x, rect.y + y },
				.color = tint,
				.uv = {
					uv0.x,
					engine::lerp(uv0.y, uv1.y, 1.0f - ((float)y / (float)rect.height)),
				}
			};
			Vertex right = {
				.pos = { rect.x + rect.width - 1, rect.y + y },
				.color = tint,
				.uv = {
					uv1.x,
					engine::lerp(uv0.y, uv1.y, 1.0f - ((float)y / (float)rect.height)),
				}
			};
			_put_line(bitmap, left, right, &image);
		}
	}

	void Renderer::_put_text(Bitmap* bitmap, Typeface* typeface, int32_t font_size, IVec2 pos, RGBA color, const std::string& text) {
		int cursor_x = pos.x;
		int cursor_y = pos.y;
		for (char character : text) {
			/* Render character */
			const engine::Glyph& glyph = typeface->glyph(font_size, character);
			for (int32_t y = 0; y < glyph.height; y++) {
				for (int32_t x = 0; x < glyph.width; x++) {
					engine::Pixel pixel = engine::Pixel::from_rgb(color);
					float alpha = (glyph.get(x, y) / 255.0f) * (color.a / 255.0f);
					bitmap->put(cursor_x + glyph.left_side_bearing + x, cursor_y + y + glyph.y_offset, pixel, alpha);
				}
			}

			/* Advance position */
			cursor_x += glyph.advance_width;
		}
	}

} // namespace engine
