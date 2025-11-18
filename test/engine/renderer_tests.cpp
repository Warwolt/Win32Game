#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>

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
	for (uint8_t x = 0; x < 100; x++) {
		renderer.draw_point(Vertex { { 20 + 2 * x, BITMAP_HEIGHT / 2 - 20 + 0 * 20 }, { 255, 0, 0, (uint8_t)std::lerp(255, 0, x / 100.0f) } });
		renderer.draw_point(Vertex { { 20 + 2 * x, BITMAP_HEIGHT / 2 - 20 + 1 * 20 }, { 0, 255, 0, (uint8_t)std::lerp(255, 0, x / 100.0f) } });
		renderer.draw_point(Vertex { { 20 + 2 * x, BITMAP_HEIGHT / 2 - 20 + 2 * 20 }, { 0, 0, 255, (uint8_t)std::lerp(255, 0, x / 100.0f) } });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
