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
		bool operator==(const Vertex& rhs) const = default;
	};

	class Renderer {
	public:
		void clear_screen(RGBA color = { 0, 0, 0, 255 });

		void draw_point(Vertex v1);
		void draw_line(Vertex v1, Vertex v2);
		void draw_rect(Rect rect, RGBA color);
		void draw_rect_fill(Rect rect, RGBA color);
		void draw_circle(IVec2 center, int32_t radius, RGBA color);
		void draw_circle_fill(IVec2 center, int32_t radius, RGBA color);
		void draw_triangle(Vertex v1, Vertex v2, Vertex v3);
		void draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3);

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
		using DrawCommand = std::variant<ClearScreen, DrawPoint, DrawLine>;
		struct CommandBatch {
			Rect rect;
			std::vector<DrawCommand> commands;
		};

		// The scratchpad is used to separate drawing the pixels from the alpha
		// blending, so that we don't have to care about overdraw. We borrow
		// `padding` in Pixel to store the alpha value.
		Bitmap m_scratchpad;
		std::vector<CommandBatch> m_batches;

		void _clear_screen(Bitmap* bitmap, RGBA color);
		void _put_point(Bitmap* bitmap, Vertex v1, bool use_alpha);
		void _put_line(Bitmap* bitmap, Vertex v1, Vertex v2, bool use_alpha);
	};

} // namespace engine
