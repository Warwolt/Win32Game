#include <gtest/gtest.h>

#include <test/helpers/parameterized_tests.h>
#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>

#include <cmath>
#include <engine/math/math_defines.h>

using namespace engine;

constexpr int BITMAP_WIDTH = 256;
constexpr int BITMAP_HEIGHT = 240;

TEST(RendererTests, ClearScreen) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	renderer.clear_screen(RGBA::turquoise());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawPoint) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	for (int x = 0; x < BITMAP_WIDTH / 2; x++) {
		IVec2 position = { 2 * x, (uint8_t)(BITMAP_HEIGHT / 2 - 20) };
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, 255 } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, 255 } });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawPoint_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	for (int x = 0; x < BITMAP_WIDTH / 2; x++) {
		IVec2 position = { 2 * x, (uint8_t)(BITMAP_HEIGHT / 2 - 20) };
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, 127 } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, 127 } });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawLine_InterpolateColors) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(Vertex { pos1, RGBA::red() }, Vertex { pos2, RGBA::green() });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawLine_InterpolateColors_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(Vertex { pos1, RGBA::red().with_alpha(0.5f) }, Vertex { pos2, RGBA::green().with_alpha(0.5f) });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawLine_FixedColors) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(pos1, pos2, RGBA::green());
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawLine_FixedColors_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(pos1, pos2, RGBA::green().with_alpha(0.5f));
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawRect) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect(rect, RGBA::green());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawRect_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect(rect, RGBA::green().with_alpha(0.5f));

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawRectFill) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect_fill(rect, RGBA::green());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawRectFill_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect_fill(rect, RGBA::green().with_alpha(0.5f));

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawCircle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle(center, 75, RGBA::green());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawCircle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle(center, 75, RGBA::green().with_alpha(0.5f));

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawCircleFill) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle_fill(center, 75, RGBA::green());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawCircleFill_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle_fill(center, 75, RGBA::green().with_alpha(0.5f));

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawTriangle_EquilateralTriangle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, RGBA::red() };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, RGBA::green() };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, RGBA::blue() };
	renderer.draw_triangle(v1, v2, v3);

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawTriangle_EquilateralTriangle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, RGBA::red().with_alpha(0.5f) };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, RGBA::green().with_alpha(0.5f) };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, RGBA::blue().with_alpha(0.5f) };
	renderer.draw_triangle(v1, v2, v3);

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawTriangleFill_EquilateralTriangle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, RGBA::red() };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, RGBA::green() };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, RGBA::blue() };
	renderer.draw_triangle_fill(v1, v2, v3);

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawTriangleFill_EquilateralTriangle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, RGBA::red().with_alpha(0.5f) };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, RGBA::green().with_alpha(0.5f) };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, RGBA::blue().with_alpha(0.5f) };
	renderer.draw_triangle_fill(v1, v2, v3);

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
