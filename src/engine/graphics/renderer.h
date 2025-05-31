#pragma once

#include <engine/graphics/color.h>
#include <engine/graphics/window.h>
#include <engine/math/ivec2.h>

#include <stdint.h>
#include <variant>
#include <vector>
#include <windows.h>

namespace engine {

	struct Rect {
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;
	};

	class Renderer {
	public:
		void clear_screen();
		void draw_pixel(int32_t x, int32_t y, Color color);
		void draw_line(IVec2 start, IVec2 end, Color color);
		void draw_rect(Rect rect, Color color);
		void draw_rect_fill(Rect rect, Color color);
		void draw_polygon(std::vector<IVec2> vertices, Color color);
		void draw_polygon_fill(std::vector<IVec2> vertices, Color color);
		void draw_circle(int32_t x, int32_t y, int32_t radius, Color color);
		void draw_circle_fill(int32_t x, int32_t y, int32_t radius, Color color);
		void render(engine::Window* window, HDC device_context);

	private:
		struct ClearScreen {};
		struct DrawPixel {
			int32_t x;
			int32_t y;
			Color color;
		};
		struct DrawLine {
			IVec2 start;
			IVec2 end;
			Color color;
		};
		struct DrawRect {
			Rect rect;
			Color color;
			bool filled;
		};
		struct DrawPolygon {
			std::vector<IVec2> vertices;
			Color color;
			bool filled;
		};
		struct DrawCircle {
			int32_t x;
			int32_t y;
			int32_t radius;
			Color color;
			bool filled;
		};

		using DrawCommand = std::variant<
			ClearScreen,
			DrawPixel,
			DrawLine,
			DrawRect,
			DrawPolygon,
			DrawCircle>;

		std::vector<DrawCommand> m_draw_commands;

		void _clear_screen(Bitmap* bitmap);
		void _put_pixel(Bitmap* bitmap, int32_t x, int32_t y, Color color);
		void _put_line(Bitmap* bitmap, IVec2 start, IVec2 end, Color color);
		void _put_rect(Bitmap* bitmap, Rect rect, Color color);
		void _put_rect_fill(Bitmap* bitmap, Rect rect, Color color);
		void _put_polygon(Bitmap* bitmap, const std::vector<IVec2>& vertices, Color color);
		void _put_polygon_fill(Bitmap* bitmap, const std::vector<IVec2>& vertices, Color color);
		void _put_circle(Bitmap* bitmap, int32_t x, int32_t y, int32_t radius, Color color);
		void _put_circle_fill(Bitmap* bitmap, int32_t x, int32_t y, int32_t radius, Color color);
	};

} // namespace engine
