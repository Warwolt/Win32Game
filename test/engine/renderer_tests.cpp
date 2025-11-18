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

	renderer.clear_screen({ 0, 127, 127, 255 });

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST(RendererTests, DrawPoint_RedGreenBlue_VaryingAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ResourceManager resources;

	for (uint8_t x = 0; x < 100; x++) {
		renderer.draw_point(Vertex { { 20 + 2 * x, 20 + 0 * 20 }, { 255, 0, 0, (uint8_t)(255 - x) } });
		renderer.draw_point(Vertex { { 20 + 2 * x, 20 + 1 * 20 }, { 0, 255, 0, (uint8_t)(255 - x) } });
		renderer.draw_point(Vertex { { 20 + 2 * x, 20 + 2 * 20 }, { 0, 0, 255, (uint8_t)(255 - x) } });
	}

	renderer.render(resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
