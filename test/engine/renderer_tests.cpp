#include <gtest/gtest.h>

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

TEST(RendererTests, DrawPoint_VaryingAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	for (int x = 0; x < BITMAP_WIDTH / 2; x++) {
		IVec2 position = { 2 * x, (uint8_t)(BITMAP_HEIGHT / 2 - 20) };
		uint8_t alpha = (uint8_t)(std::lerp(255.0f, 0.0f, (float)(x) / (BITMAP_WIDTH / 2 - 1)));
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, alpha } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, alpha } });
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

TEST(RendererTests, DrawLine_FixedColors) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(pos1, pos2, RGBA::purple());
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
