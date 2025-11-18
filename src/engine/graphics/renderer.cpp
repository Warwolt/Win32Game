#include <engine/graphics/renderer.h>

#include <engine/container/match_variant.h>
#include <engine/debug/profiling.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/font.h>
#include <engine/graphics/image.h>
#include <engine/graphics/rect.h>
#include <engine/math/math.h>
#include <engine/utility/string_utility.h>

#include <cmath>
#include <utility>

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
		IVec2 prev_point = IVec2 { octant_points[0].x, octant_points[0].y };
		std::vector<IVec2> half_circle_points = { prev_point };
		for (IVec2 point : octant_points) {
			int32_t delta_y = point.y - prev_point.y;
			if (delta_y == 0) {
				half_circle_points.back() = point;
			}
			else {
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

	Renderer Renderer::with_bitmap(int32_t width, int32_t height) {
		Renderer renderer;
		renderer.m_bitmap = Bitmap::with_size(width, height);
		return renderer;
	}

	void Renderer::add_tag(std::string tag) {
		m_current_tag = tag;
	}

	void Renderer::clear_screen(RGBA color) {
		m_draw_data.push_back(DrawData { ClearScreen { color }, _take_current_tag() });
	}

	void Renderer::draw_point(Vertex v1) {
		m_draw_data.push_back(DrawData { DrawPoint { v1 }, _take_current_tag() });
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
		m_draw_data.push_back(DrawData { DrawLine { v1, v2 }, _take_current_tag() });
	}

	void Renderer::draw_line(IVec2 pos1, IVec2 pos2, RGBA color) {
		m_draw_data.push_back(DrawData { DrawLine { Vertex { .pos = pos1, .color = color }, Vertex { .pos = pos2, .color = color } }, _take_current_tag() });
	}

	void Renderer::draw_rect(Rect rect, RGBA color) {
		m_draw_data.push_back(DrawData { DrawRect { rect, color, false }, _take_current_tag() });
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
		m_draw_data.push_back(DrawData { DrawRect { rect, color, true }, _take_current_tag() });
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
		m_draw_data.push_back(DrawData { DrawCircle { center, radius, color, false }, _take_current_tag() });
	}

	void Renderer::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
		m_draw_data.push_back(DrawData { DrawCircle { center, radius, color, true }, _take_current_tag() });
	}

	void Renderer::draw_triangle(Vertex v1, Vertex v2, Vertex v3) {
		m_draw_data.push_back(DrawData { DrawTriangle { v1, v2, v3, false }, _take_current_tag() });
	}

	void Renderer::draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3) {
		m_draw_data.push_back(DrawData { DrawTriangle { v1, v2, v3, true }, _take_current_tag() });
	}

	void Renderer::draw_image(ImageID image_id, IVec2 pos, DrawImageOptions options) {
		m_draw_data.push_back(DrawData { DrawImage { image_id, Rect { pos.x, pos.y }, options }, _take_current_tag() });
	}

	void Renderer::draw_image_scaled(ImageID image_id, Rect rect, DrawImageOptions options) {
		m_draw_data.push_back(DrawData { DrawImage { image_id, rect, options }, _take_current_tag() });
	}

	void Renderer::draw_text(FontID font_id, int32_t font_size, Rect rect, RGBA color, std::string text, DrawTextOptions options) {
		m_draw_data.push_back(DrawData { DrawText { font_id, font_size, rect, color, text, options }, _take_current_tag() });
	}

	const Bitmap& Renderer::bitmap() {
		return m_bitmap;
	}

	IVec2 Renderer::screen_resolution() const {
		return m_bitmap.size();
	}

	void Renderer::render(const ResourceManager& resources) {
		CPUProfilingScope_Render();
		TracyPlot("DrawCommands", (int64_t)m_draw_data.size());

		/* Run commands */
		for (const auto& [command, tag] : m_draw_data) {
			if (!tag.empty()) {
				TracyMessage(tag.data(), tag.size());
			}
			MATCH_VARIANT(command) {
				MATCH_CASE(ClearScreen, color) {
					_clear_screen(&m_bitmap, color);
				}
				MATCH_CASE(DrawPoint, v1) {
					_put_point(&m_bitmap, v1);
				}
				MATCH_CASE(DrawLine, v1, v2) {
					_put_line(&m_bitmap, v1, v2, nullptr);
				}
				MATCH_CASE(DrawRect, rect, color, filled) {
					if (filled) {
						_put_rect_fill(&m_bitmap, rect, color);
					}
					else {
						_put_rect(&m_bitmap, rect, color);
					}
				}
				MATCH_CASE(DrawCircle, center, radius, color, filled) {
					if (filled) {
						_put_circle_fill(&m_bitmap, center, radius, color);
					}
					else {
						_put_circle(&m_bitmap, center, radius, color);
					}
				}
				MATCH_CASE(DrawTriangle, v1, v2, v3, filled) {
					if (filled) {
						_put_triangle_fill(&m_bitmap, v1, v2, v3);
					}
					else {
						_put_triangle(&m_bitmap, v1, v2, v3);
					}
				}
				MATCH_CASE(DrawImage, image_id, rect, const_options) {
					DrawImageOptions options = const_options;
					const Image& image = resources.image(image_id);
					if (rect.empty()) {
						if (options.clip.empty()) {
							options.clip = Rect { 0, 0, image.width, image.height };
						}
						_put_image(&m_bitmap, image, rect.pos(), options);
					}
					else {
						_put_image_scaled(&m_bitmap, image, rect, options);
					}
				}
				MATCH_CASE(DrawText, font_id, font_size, rect, color, text, options) {
					const Typeface& font = resources.typeface(font_id);
					_put_text(&m_bitmap, font, font_size, rect, color, text, options);
				}
			}
		}
		m_draw_data.clear();
	}

	std::string Renderer::_take_current_tag() {
		if (!m_current_tag.empty()) {
			m_last_tag = m_current_tag;
		}
		return std::exchange(m_current_tag, std::string());
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		CPUProfilingScope_Render();
		bitmap->clear(Pixel::from_rgb(color));
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1) {
		CPUProfilingScope_Render();
		Pixel pixel = { .b = v1.color.b, .g = v1.color.g, .r = v1.color.r, .padding = v1.color.a };
		bitmap->put(v1.pos.x, v1.pos.y, pixel, v1.color.a / 255.0f);
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2, const Image* image) {
		CPUProfilingScope_Render();
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
		CPUProfilingScope_Render();
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
		CPUProfilingScope_Render();
		Pixel pixel = Pixel::from_rgb(color);
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			for (int32_t x = rect.x; x < rect.x + rect.width; x++) {
				bitmap->put(x, y, pixel, color.a / 255.0f);
			}
		}
	}

	void Renderer::_put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color) {
		CPUProfilingScope_Render();
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
		CPUProfilingScope_Render();
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
		CPUProfilingScope_Render();
		_put_line(bitmap, v1, v2, nullptr);
		_put_line(bitmap, v1, v3, nullptr);
		_put_line(bitmap, v2, v3, nullptr);
	}

	void Renderer::_put_triangle_fill(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3) {
		CPUProfilingScope_Render();
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

	void Renderer::_put_image_scaled(Bitmap* bitmap, const Image& image, Rect rect, DrawImageOptions options) {
		CPUProfilingScope_Render();
		/* UV coordinates */
		if (options.clip.empty()) {
			options.clip.width = rect.width;
			options.clip.height = rect.height;
		}
		// bottom left
		Vec2 uv0 = {
			.x = engine::clamp((float)options.clip.x / (float)(image.width - 1), 0.0f, 1.0f),
			.y = engine::clamp((float)options.clip.y / (float)(image.height), 0.0f, 1.0f),
		};
		// top right
		Vec2 uv1 = {
			.x = engine::clamp((float)(options.clip.x + options.clip.width - 1) / (float)(image.width - 1), 0.0f, 1.0f),
			.y = engine::clamp((float)(options.clip.y + options.clip.height) / (float)(image.height), 0.0f, 1.0f),
		};

		/* Draw image line by line */
		RGBA color = RGBA::lerp(RGBA::white(), options.tint, options.tint.a / 255.0f).with_alpha((uint8_t)(255.0f * options.alpha));
		for (int32_t y = 0; y < rect.height; y++) {
			Vertex left = {
				.pos = { rect.x, rect.y + y },
				.color = color,
				.uv = {
					options.flip_h ? uv1.x : uv0.x,
					engine::lerp(uv0.y, uv1.y, 1.0f - ((float)y / (float)rect.height)),
				}
			};
			Vertex right = {
				.pos = { rect.x + rect.width - 1, rect.y + y },
				.color = color,
				.uv = {
					options.flip_h ? uv0.x : uv1.x,
					engine::lerp(uv0.y, uv1.y, 1.0f - ((float)y / (float)rect.height)),
				}
			};
			_put_line(bitmap, left, right, &image);
		}
	}

	void Renderer::_put_image(Bitmap* bitmap, const Image& image, IVec2 pos, DrawImageOptions options) {
		CPUProfilingScope_Render();
		int32_t y_end = engine::min(options.clip.height, image.height);
		int32_t x_end = engine::min(options.clip.width, image.width);
		for (int32_t y_offset = 0; y_offset < y_end; y_offset++) {
			for (int32_t x_offset = 0; x_offset < x_end; x_offset++) {
				RGBA color = RGBA::tint(image.get(options.clip.x + x_offset, options.clip.y + y_offset), options.tint);
				int32_t x = options.flip_h ? pos.x + (options.clip.width - 1 - x_offset) : pos.x + x_offset;
				int32_t y = options.flip_v ? pos.y + (options.clip.height - 1 - y_offset) : pos.y + y_offset;
				bitmap->put(x, y, Pixel::from_rgb(color), color.a / 255.0f * options.alpha);
			}
		}
	}

	void Renderer::_put_text(Bitmap* bitmap, const Typeface& font, int32_t font_size, Rect rect, RGBA color, const std::string& text, DrawTextOptions options) {
		const int32_t ascent = font.ascent(font_size);
		const int32_t space_width = font.glyph(font_size, ' ').advance_width;

		int32_t cursor_x = 0;
		int32_t cursor_y = ascent;

		/* Bounding rect defaults */
		if (rect.width == 0) {
			rect.width = font.text_width(font_size, text);
		}
		if (rect.height == 0) {
			rect.height = font_size + 1;
		}

		/* Render text row-by-row */
		const std::vector<std::string> words = split_string_into_words(text);
		auto line_start = words.begin();
		while (line_start != words.end() && cursor_y < rect.height) {
			/* Find how many words fit current row */
			int line_width = 0;
			auto line_end = line_start;
			for (; line_end != words.end(); ++line_end) {
				const std::string& word = *line_end;
				const int word_width = font.text_width(font_size, word);
				const int needed_width = (line_width > 0 ? line_width + space_width : line_width) + word_width;
				if (needed_width > rect.width) {
					break;
				}
				line_width = needed_width;
			}

			/* Compute word position based on alignment */
			const int row_remainder = rect.width - line_width;
			switch (options.h_alignment) {
				case HorizontalAlignment::Left: cursor_x = 0; break;
				case HorizontalAlignment::Center: cursor_x = row_remainder / 2; break;
				case HorizontalAlignment::Right: cursor_x = row_remainder; break;
			}

			/* Put all words in current row */
			for (auto it = line_start; it != line_end; ++it) {
				const std::string& word = *it;
				for (char character : word) {
					/* Render character */
					const engine::Glyph& glyph = font.glyph(font_size, character);
					for (int32_t y = 0; y < glyph.height; y++) {
						for (int32_t x = 0; x < glyph.width; x++) {
							engine::Pixel pixel = engine::Pixel::from_rgb(color);
							float alpha = (glyph.get(x, y) / 255.0f) * (color.a / 255.0f);
							int32_t pixel_x = rect.x + cursor_x + glyph.left_side_bearing + x;
							int32_t pixel_y = rect.y + cursor_y + y + glyph.y_offset;
							bitmap->put(pixel_x, pixel_y, pixel, alpha);
						}
					}

					/* Go to next column */
					cursor_x += glyph.advance_width;
				}

				/* Add space between words */
				cursor_x += space_width;
			}

			/* Advance to next row */
			cursor_y += ascent;
			line_start = line_end;
		}

		/* Debug render bounding rect */
		if (options.debug_draw_box) {
			_put_rect(bitmap, rect, RGBA::green());
		}
	}

} // namespace engine
