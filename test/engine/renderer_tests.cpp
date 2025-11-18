#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>

#include <cmath>
#include <engine/math/math_defines.h>

using namespace engine;

constexpr int BITMAP_WIDTH = 256;
constexpr int BITMAP_HEIGHT = 240;

TEST(RendererTests, ClearScreen_Turquoise) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	renderer.clear_screen(RGBA::turquoise());

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawPoint_RedGreenBlue_VaryingAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	renderer.clear_screen(RGBA::turquoise());
	for (int x = 0; x < BITMAP_WIDTH; x++) {
		IVec2 position = { x, (uint8_t)((BITMAP_HEIGHT / 2 - 20) + 30 * sinf(2 * (float)M_PI * (float)x / BITMAP_WIDTH)) };
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, (uint8_t)std::lerp(255, 0, (float)x / BITMAP_WIDTH) } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, (uint8_t)std::lerp(255, 0, (float)x / BITMAP_WIDTH) } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 2 * 20 }, { 0, 0, 255, (uint8_t)std::lerp(255, 0, (float)x / BITMAP_WIDTH) } });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
