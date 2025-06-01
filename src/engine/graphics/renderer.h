#pragma once

#include <engine/graphics/rgba.h>
#include <engine/graphics/rect.h>
#include <engine/graphics/window.h>
#include <engine/math/ivec2.h>

#include <stdint.h>
#include <variant>
#include <vector>
#include <windows.h>

namespace engine {

	class Renderer {
	public:
		void clear_screen(RGBA color = { 0, 0, 0, 255 });
		void draw_point(IVec2 point, RGBA color);
		void draw_line(IVec2 start, IVec2 end, RGBA color);
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
			IVec2 start;
			IVec2 end;
			RGBA color;
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
		void _put_line(Bitmap* bitmap, IVec2 start, IVec2 end, RGBA color);
		void _put_rect(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_rect_fill(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_polygon(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color);
		void _put_polygon_fill(Bitmap* bitmap, const std::vector<IVec2>& vertices, RGBA color);
		void _put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
		void _put_circle_fill(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
	};

} // namespace engine
