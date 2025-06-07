#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/graphics/rect.h>
#include <engine/graphics/rgba.h>
#include <engine/math/ivec2.h>

#include <variant>
#include <vector>

namespace engine {

	struct Vertex {
		IVec2 pos;
		RGBA color;
	};

	class Renderer {
	public:
		void clear_screen(RGBA color = { 0, 0, 0, 255 });
		void draw_point(IVec2 point, RGBA color);
		void draw_line(Vertex start, Vertex end);
		void draw_rect(Rect rect, RGBA color);
		void draw_rect_fill(Rect rect, RGBA color);
		void draw_circle(IVec2 center, int32_t radius, RGBA color);
		void draw_circle_fill(IVec2 center, int32_t radius, RGBA color);
		void render(Bitmap* bitmap);

	private:
		struct ClearScreen {
			RGBA color;
		};
		struct DrawPoint {
			Vertex v1;
		};
		struct DrawLine {
			Vertex v1;
			Vertex v2;
		};
		struct DrawTriangle {
			Vertex v1;
			Vertex v2;
			Vertex v3;
		};
		using DrawCommand = std::variant<ClearScreen, DrawPoint, DrawLine, DrawTriangle>;
		std::vector<DrawCommand> m_commands;

		void _clear_screen(Bitmap* bitmap, RGBA color);
		void _put_point(Bitmap* bitmap, Vertex v1);
		void _put_line(Bitmap* bitmap, Vertex v1, Vertex v2);
		void _put_triangle(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3);
	};

} // namespace engine
