#include <engine/graphics/renderer.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/font.h>
#include <engine/graphics/image.h>
#include <engine/math/math.h>

#include "renderer.h"
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
		m_batches.push_back(CommandBatch { .commands = { ClearScreen { color } } });
	}

	void Renderer::draw_point(Vertex v1) {
		m_batches.push_back(CommandBatch {
			.commands = {
				DrawPoint { v1 },
			},
		});
	}

	void Renderer::draw_line(Vertex v1, Vertex v2) {
		m_batches.push_back(CommandBatch {
			.commands = {
				DrawLine { v1, v2 },
			},
		});
	}

	void Renderer::draw_rect(Rect rect, RGBA color) {
		m_batches.push_back(CommandBatch {
			.commands = {
				DrawRect { rect, color, false },
			},
		});
	}

	void Renderer::draw_rect_fill(Rect rect, RGBA color) {
		CommandBatch batch = { .rect = rect };
		for (int32_t y = rect.y; y < rect.y + rect.height; y++) {
			Vertex left = { .pos = { rect.x, y }, .color = color };
			Vertex right = { .pos = { rect.x + rect.width - 1, y }, .color = color };
			batch.commands.push_back(DrawLine { left, right });
		}
		m_batches.push_back(batch);
	}

	void Renderer::draw_circle(IVec2 center, int32_t radius, RGBA color) {
		CommandBatch batch = {
			.rect = Rect {
				.x = center.x - radius,
				.y = center.y - radius,
				.width = 2 * radius + 1,
				.height = 2 * radius + 1,
			},
		};
		for (IVec2 point : circle_octant_points(radius)) {
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { point.x, point.y }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { point.y, point.x }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { point.y, -point.x }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { point.x, -point.y }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { -point.x, point.y }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { -point.y, point.x }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { -point.y, -point.x }, .color = color } });
			batch.commands.push_back(DrawPoint { Vertex { .pos = center + IVec2 { -point.x, -point.y }, .color = color } });
		}
		m_batches.push_back(batch);
	}

	void Renderer::draw_circle_fill(IVec2 center, int32_t radius, RGBA color) {
		CommandBatch batch = {
			.rect = Rect {
				.x = center.x - radius,
				.y = center.y - radius,
				.width = 2 * radius + 1,
				.height = 2 * radius + 1,
			},
		};
		for (IVec2 point : circle_octant_points(radius)) {
			for (int32_t x = -point.x; x <= point.x; x++) {
				batch.commands.emplace_back(DrawLine {
					.v1 = { .pos = center + IVec2 { x, point.y }, .color = color },
					.v2 = { .pos = center + IVec2 { x, -point.y }, .color = color },
				});
			}
			for (int32_t y = -point.y; y <= point.y; y++) {
				batch.commands.emplace_back(DrawLine {
					.v1 = { .pos = center + IVec2 { y, point.x }, .color = color },
					.v2 = { .pos = center + IVec2 { y, -point.x }, .color = color },
				});
			}
		}
		m_batches.push_back(batch);
	}

	void Renderer::draw_triangle(Vertex v1, Vertex v2, Vertex v3) {
		int32_t min_x = std::min({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t min_y = std::min({ v1.pos.y, v2.pos.y, v3.pos.y });
		int32_t max_x = std::max({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t max_y = std::max({ v1.pos.y, v2.pos.y, v3.pos.y });
		m_batches.push_back(CommandBatch {
			.rect = Rect {
				min_x,
				min_y,
				max_x - min_x + 1,
				max_y - min_y + 1,
			},
			.commands = {
				DrawLine { v1, v2 },
				DrawLine { v1, v3 },
				DrawLine { v2, v3 },
			},
		});
	}

	void Renderer::draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3) {
		struct TriangleEdge {
			int32_t x0;
			int32_t y0;
			int32_t y1;
			float inv_slope;
		};

		auto make_triangle_edge = [](Vertex a, Vertex b) -> TriangleEdge {
			return TriangleEdge {
				.x0 = a.pos.x,
				.y0 = a.pos.y,
				.y1 = b.pos.y,
				.inv_slope = (float)(b.pos.x - a.pos.x) / (float)(b.pos.y - a.pos.y),
			};
		};

		std::array<TriangleEdge, 3> edges = {
			make_triangle_edge(v1, v2),
			make_triangle_edge(v1, v3),
			make_triangle_edge(v2, v3),
		};

		int32_t min_x = std::min({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t min_y = std::min({ v1.pos.y, v2.pos.y, v3.pos.y });
		int32_t max_x = std::max({ v1.pos.x, v2.pos.x, v3.pos.x });
		int32_t max_y = std::max({ v1.pos.y, v2.pos.y, v3.pos.y });

		CommandBatch batch = {
			.rect = Rect {
				min_x,
				min_y,
				max_x - min_x + 1,
				max_y - min_y + 1,
			},
		};

		auto triangle_area = [](IVec2 a, IVec2 b) -> float {
			return std::abs((float)(a.x * b.y - a.y * b.x) / 2.0f);
		};

		float area_v1v2v3 = triangle_area(v2.pos - v1.pos, v3.pos - v1.pos);

		for (int32_t y = min_y; y <= max_y; y++) {
			/* Get intersections betwen scanline and lines extended from triangle sides */
			std::array<int32_t, 3> xs = {
				(int32_t)std::round(edges[0].inv_slope * (y - edges[0].y0) + edges[0].x0),
				(int32_t)std::round(edges[1].inv_slope * (y - edges[1].y0) + edges[1].x0),
				(int32_t)std::round(edges[2].inv_slope * (y - edges[2].y0) + edges[2].x0),
			};

			/* Discard line intersections outside of triangle */
			auto is_out_of_bounds = [min_x, max_x](int32_t x) { return x < min_x || x > max_x; };
			auto last = std::remove_if(xs.begin(), xs.end(), is_out_of_bounds);
			std::sort(xs.begin(), xs.end());
			IVec2 p1 = { xs[0], y };
			IVec2 p2 = { xs[1], y };

			/* Interpolate colors */
			// left color
			float area_v1v2p1 = triangle_area(v2.pos - v1.pos, p1 - v1.pos);
			float area_v1v3p1 = triangle_area(v3.pos - v1.pos, p1 - v1.pos);
			float area_v2v3p1 = triangle_area(v3.pos - v2.pos, p1 - v2.pos);
			float v1_t1 = area_v2v3p1 / area_v1v2v3;
			float v2_t1 = area_v1v3p1 / area_v1v2v3;
			float v3_t1 = area_v1v2p1 / area_v1v2v3;
			RGBA color1 = v1_t1 * v1.color + v2_t1 * v2.color + v3_t1 * v3.color;

			// right color
			float area_v1v2p2 = triangle_area(v2.pos - v1.pos, p2 - v1.pos);
			float area_v1v3p2 = triangle_area(v3.pos - v1.pos, p2 - v1.pos);
			float area_v2v3p2 = triangle_area(v3.pos - v2.pos, p2 - v2.pos);
			float v1_t2 = area_v2v3p2 / area_v1v2v3;
			float v2_t2 = area_v1v3p2 / area_v1v2v3;
			float v3_t2 = area_v1v2p2 / area_v1v2v3;
			RGBA color2 = v1_t2 * v1.color + v2_t2 * v2.color + v3_t2 * v3.color;

			/* Draw line */
			batch.commands.push_back(DrawLine {
				.v1 = Vertex { .pos = { xs[0], y }, .color = color1 },
				.v2 = Vertex { .pos = { xs[1], y }, .color = color2 },
			});
		}

		m_batches.push_back(batch);
	}

	void Renderer::draw_image(ImageID image_id, Rect rect, Rect clip, RGBA tint) {
		CommandBatch batch = { .rect = rect, .image_id = image_id };

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
			batch.commands.push_back(DrawLine { left, right });
		}

		m_batches.push_back(batch);
	}

	void Renderer::draw_text(FontID font_id, int32_t font_size, IVec2 pos, RGBA color, std::string text) {
		m_batches.push_back(CommandBatch {
			.commands = {
				DrawText { font_id, font_size, pos, color, text },
			},
		});
	}

	void Renderer::render(Bitmap* bitmap, ResourceManager* resources) {
		/* Keep scratchpad size in sync with bitmap */
		m_scratchpad.resize(bitmap->width(), bitmap->height());

		/* Run commands */
		for (const CommandBatch& batch : m_batches) {
			const Image* image = batch.image_id ? &resources->image(*batch.image_id) : nullptr;
			if (batch.rect.empty()) {
				/* Draw directly to bitmap */
				for (const DrawCommand& command : batch.commands) {
					if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
						auto& [color] = *clear_screen;
						_clear_screen(bitmap, color);
					}
					if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
						auto& [v1] = *draw_point;
						_put_point(bitmap, v1, true);
					}
					if (auto* draw_line = std::get_if<DrawLine>(&command)) {
						auto& [v1, v2] = *draw_line;
						_put_line(bitmap, v1, v2, image, true);
					}
					if (auto* draw_rect = std::get_if<DrawRect>(&command)) {
						auto& [rect, color, filled] = *draw_rect;
						if (filled) {
						}
						else {
							_put_rect(bitmap, rect, color);
						}
					}
					if (auto* draw_text = std::get_if<DrawText>(&command)) {
						auto& [font_id, font_size, pos, color, text] = *draw_text;
						Typeface& typeface = resources->font(font_id);
						_put_text(bitmap, &typeface, font_size, pos, color, text);
					}
				}
			}
			else {
				/* Clear scratch pad area */
				m_scratchpad.clear_section(Pixel { 0, 0, 0, 0 }, batch.rect);

				/* Draw onto scratch pad */
				for (const DrawCommand& command : batch.commands) {
					if (auto* draw_point = std::get_if<DrawPoint>(&command)) {
						_put_point(&m_scratchpad, draw_point->v1, false);
					}
					if (auto* draw_line = std::get_if<DrawLine>(&command)) {
						_put_line(&m_scratchpad, draw_line->v1, draw_line->v2, image, false);
					}
				}

				/* Draw scratch pad onto bitmap */
				for (int32_t y = batch.rect.y; y < batch.rect.y + batch.rect.height; y++) {
					for (int32_t x = batch.rect.x; x < batch.rect.x + batch.rect.width; x++) {
						Pixel scratchpad_pixel = m_scratchpad.get(x, y);
						float alpha = scratchpad_pixel.padding / 255.0f;
						bitmap->put(x, y, scratchpad_pixel, alpha);
					}
				}
			}
		}

		/* Clear commands */
		m_batches.clear();
	}

	void Renderer::_clear_screen(Bitmap* bitmap, RGBA color) {
		bitmap->clear(Pixel::from_rgb(color));
	}

	void Renderer::_put_point(Bitmap* bitmap, Vertex v1, bool use_alpha) {
		Pixel pixel = { .b = v1.color.b, .g = v1.color.g, .r = v1.color.r, .padding = v1.color.a };
		bitmap->put(v1.pos.x, v1.pos.y, pixel, use_alpha ? v1.color.a / 255.0f : 1.0f);
	}

	void Renderer::_put_line(Bitmap* bitmap, Vertex v1, Vertex v2, const Image* image, bool use_alpha) {
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
			cursor.color = image
				? image->sample(Vec2::lerp(v1.uv, v2.uv, t)) * RGBA::lerp(v1.color, v2.color, t)
				: RGBA::lerp(v1.color, v2.color, t);
			Pixel pixel = { .b = cursor.color.b, .g = cursor.color.g, .r = cursor.color.r, .padding = cursor.color.a };
			bitmap->put(cursor.pos.x, cursor.pos.y, pixel, use_alpha ? cursor.color.a / 255.0f : 1.0f);

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

		_put_line(bitmap, top_start, top_end, nullptr, true);
		_put_line(bitmap, bottom_start, bottom_end, nullptr, true);
		_put_line(bitmap, left_start, left_end, nullptr, true);
		_put_line(bitmap, right_start, right_end, nullptr, true);
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
