#pragma once

#include <engine/debug/filename_from_path.h>
#include <engine/graphics/bitmap.h>
#include <engine/graphics/font_id.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/graphics/rgba.h>
#include <engine/math/ivec2.h>

#include <format>
#include <string>
#include <variant>
#include <vector>

// Adds a tag to the renderer for the current file and line
#define RENDERER_LOG(renderer, message) \
	renderer->add_tag(std::format("{}:{}: {}", filename_from_path(__FILE__), __LINE__, message))

namespace engine {

	class ResourceManager;
	struct Image;
	class Font;

	struct Vertex {
		IVec2 pos;
		RGBA color;
		Vec2 uv; // bottom left (0,0), top right (1,1)
		bool operator==(const Vertex& rhs) const = default;
	};

	struct DrawImageOptions {
		Rect clip = {};
		bool flip_h = false;
		bool flip_v = false;
		float alpha = 1.0f;
		RGBA tint = RGBA::white();
	};

	enum class HorizontalTextAlignment {
		Left,
		Center,
		Right,
	};

	struct DrawTextOptions {
		HorizontalTextAlignment h_alignment = HorizontalTextAlignment::Left;
		bool debug_draw_box = false;
	};

	class Renderer {
	public:
		Renderer() = default;
		static Renderer with_bitmap(int32_t width, int32_t height);

		// tags next draw command, shows up in Tracy
		void add_tag(std::string tag);

		void clear_screen(RGBA color = { 0, 0, 0, 255 });
		void draw_point(Vertex v1);
		void draw_line(Vertex v1, Vertex v2);
		void draw_line(IVec2 pos1, IVec2 pos2, RGBA color);
		void draw_rect(Rect rect, RGBA color);
		void draw_rect_fill(Rect rect, RGBA color);
		void draw_circle(IVec2 center, int32_t radius, RGBA color);
		void draw_circle_fill(IVec2 center, int32_t radius, RGBA color);
		void draw_triangle(Vertex v1, Vertex v2, Vertex v3);
		void draw_triangle_fill(Vertex v1, Vertex v2, Vertex v3);
		void draw_image(ImageID image_id, IVec2 pos, DrawImageOptions options = {});
		void draw_image_scaled(ImageID image_id, Rect rect, DrawImageOptions options = {});
		void draw_text(FontID font_id, int32_t font_size, Rect rect, RGBA color, std::string text, DrawTextOptions options = {});

		const Bitmap& bitmap();
		IVec2 screen_resolution() const;

		void render(ResourceManager* resources);

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
		struct DrawRect {
			Rect rect;
			RGBA color;
			bool filled;
		};
		struct DrawCircle {
			IVec2 center;
			int32_t radius;
			RGBA color;
			bool filled;
		};
		struct DrawTriangle {
			Vertex v1;
			Vertex v2;
			Vertex v3;
			bool filled;
		};
		struct DrawImage {
			ImageID image_id;
			Rect rect;
			DrawImageOptions options;
		};
		struct DrawText {
			FontID font_id;
			int32_t font_size;
			Rect rect;
			RGBA color;
			std::string text;
			DrawTextOptions options;
		};
		using DrawCommand = std::variant<
			ClearScreen,
			DrawPoint,
			DrawLine,
			DrawRect,
			DrawCircle,
			DrawTriangle,
			DrawImage,
			DrawText>;

		struct DrawData {
			DrawCommand command;
			std::string tag; // meta data for what's being drawn
		};

		Bitmap m_bitmap;
		std::string m_last_tag; // for debuggin
		std::string m_current_tag;
		std::vector<DrawData> m_draw_data;

		std::string _take_current_tag();
		void _clear_screen(Bitmap* bitmap, RGBA color);
		void _put_point(Bitmap* bitmap, Vertex v1);
		void _put_line(Bitmap* bitmap, Vertex v1, Vertex v2, const Image* image);
		void _put_rect(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_rect_fill(Bitmap* bitmap, Rect rect, RGBA color);
		void _put_circle(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
		void _put_circle_fill(Bitmap* bitmap, IVec2 center, int32_t radius, RGBA color);
		void _put_triangle(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3);
		void _put_triangle_fill(Bitmap* bitmap, Vertex v1, Vertex v2, Vertex v3);
		void _put_image(Bitmap* bitmap, const Image& image, IVec2 pos, DrawImageOptions options);
		void _put_image_scaled(Bitmap* bitmap, const Image& image, Rect rect, DrawImageOptions options);
		void _put_text(Bitmap* bitmap, Font* typeface, int32_t font_size, Rect rect, RGBA color, const std::string& text, DrawTextOptions options);
	};

} // namespace engine
