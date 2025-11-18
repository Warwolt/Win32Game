#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>

using namespace engine;

TEST(RendererTests, ClearScreen_Turquoise) {
	Renderer renderer = Renderer::with_bitmap(256, 240);
	ResourceManager resources;

	renderer.clear_screen({ 0, 127, 127, 255 });
	renderer.render(resources);

	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
