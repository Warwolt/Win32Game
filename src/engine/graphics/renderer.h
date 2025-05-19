#pragma once

#include <engine/graphics/color.h>
#include <engine/graphics/window.h>
#include <engine/math/ivec2.h>

#include <stdint.h>
#include <variant>
#include <vector>
#include <windows.h>

namespace engine {

	class Renderer {
	public:
		void clear_screen();
		void draw_pixel(int32_t x, int32_t y, Color color);
		void draw_line(IVec2 start, IVec2 end, Color color);
		void render(engine::Window* window, HDC device_context);

	private:
		struct ClearScreen {
		};
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
		using DrawCommand = std::variant<
			ClearScreen,
			DrawPixel,
			DrawLine>;
		std::vector<DrawCommand> m_draw_commands;

		void _clear_screen(engine::Bitmap* bitmap);
		void _put_pixel(engine::Bitmap* bitmap, int32_t x, int32_t y, Color color);
		void _put_line(engine::Bitmap* bitmap, IVec2 start, IVec2 end, Color color);
	};

} // namespace engine
