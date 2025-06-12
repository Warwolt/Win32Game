#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/graphics/rect.h>
#include <engine/graphics/rgba.h>
#include <engine/math/ivec2.h>
#include <engine/graphics/image_id.h>

#include <variant>
#include <vector>
#include <optional>

namespace engine {

	class ResourceManager;
	struct Image;

	struct Vertex {
		IVec2 pos;
		RGBA color;
		Vec2 uv; // bottom left (0,0), top right (1,1)
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
		void draw_image(ImageID image_id, Rect rect, Rect clip = {});

		void render(Bitmap* bitmap, const ResourceManager& resources);

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
			std::optional<ImageID> image_id;
			std::vector<DrawCommand> commands;
		};

		// The scratchpad is used to separate drawing the pixels from the alpha
		// blending, so that we don't have to care about overdraw. We borrow
		// `padding` in Pixel to store the alpha value.
		Bitmap m_scratchpad;
		std::vector<CommandBatch> m_batches;

		void _clear_screen(Bitmap* bitmap, RGBA color);
		void _put_point(Bitmap* bitmap, Vertex v1, bool use_alpha);
		void _put_line(Bitmap* bitmap, Vertex v1, Vertex v2, const Image* image, bool use_alpha);
	};

} // namespace engine
