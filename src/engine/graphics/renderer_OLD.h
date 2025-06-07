#pragma once

#include <engine/graphics/rect.h>
#include <engine/graphics/rgba.h>
#include <engine/graphics/window.h>
#include <engine/math/ivec2.h>

#include <stdint.h>
#include <variant>
#include <vector>
#include <windows.h>

namespace engine {

	struct Vertex {
		IVec2 pos;
		RGBA color;
	};

	// What API should we have?
	// - To interpolate color and UV between vertices we HAVE to use triangles
	// - It's extremely convinient to be able to draw rectangles and circles even if they're a single color
	//
	// There seems to be basically two layers here:
	// - A software rasterizer that, like OpenGL, only wants to work with points, lines and triangles
	// - A draw API that has several useful shapes
	//
	// We _could_ have two classes here, a "DrawAPI" class and a "Renderer_OLD"
	// class, where the latter only does triangles BUT that might be kinda annoying.
	//
	// Instead we might just let the public methods of Renderer_OLD be the draw API
	// and then have private methods that just work with points, lines and
	// triangles.

	class Renderer_OLD {
	public:
		void clear_screen(RGBA color = { 0, 0, 0, 255 });
		void draw_point(IVec2 point, RGBA color);
		void draw_line(Vertex start, Vertex end);

		// TODO: we need to change the implementation of draw_rect and
		// draw_rect_fill to allow for interpolation.
		// For draw_rect we can probably just use draw_line, but for
		// the fill case we should figure out how to draw triangles
		// with color interpolation, and then just construct the filled
		// rectangle out of two triangles.
		void draw_rect(Rect rect, RGBA color);
		void draw_rect_fill(Rect rect, RGBA color);

		void draw_polygon(std::vector<IVec2> vertices, RGBA color);
		void draw_polygon_fill(std::vector<IVec2> vertices, RGBA color);
		void draw_circle(IVec2 center, int32_t radius, RGBA color);
		void draw_circle_fill(IVec2 center, int32_t radius, RGBA color);

		void render(Bitmap* bitmap);

	private:
		struct ClearScreen {
			RGBA color;
		};
		struct DrawPoint {
			IVec2 point;
			RGBA color;
		};
		struct DrawLine {
			Vertex start;
			Vertex end;
		};
		struct DrawRect {
			Rect rect;
			RGBA color;
			bool filled;
		};
		struct DrawPolygon {
			std::vector<IVec2> vertices;
			RGBA color;
			bool filled;
		};
		struct DrawCircle {
			IVec2 center;
			int32_t radius;
			RGBA color;
			bool filled;
		};

		using DrawCommand = std::variant<
			ClearScreen,
			DrawPoint,
			DrawLine,
			DrawRect,
			DrawPolygon,
			DrawCircle>;

		std::vector<DrawCommand> m_draw_commands;

		void _clear_screen(Bitmap* bitmap, RGBA color);
		void _put_point(Bitmap* bitmap, IVec2 point, RGBA color);
		void _put_line_OLD(Bitmap* bitmap, IVec2 start, IVec2 end, RGBA color);
		void _put_line(Bitmap* bitmap, Vertex start, Vertex end);
		void _put_rect(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_rect_fill(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_polygon(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color);
		void _put_polygon_fill(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color);
		void _put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
		void _put_circle_fill(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
	};

} // namespace engine
