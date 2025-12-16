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
constexpr int TEST_FONT_SIZE = 16;
constexpr char LOREM_IPSUM[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer tempor placerat tellus in dapibus. Etiam suscipit ex risus, eu faucibus ipsum vestibulum id. Vivamus imperdiet dui id ligula pretium, at aliquam eros ornare. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.";

class RendererTests : public testing::Test {
public:
	ResourceManager m_resources;
	ImageID m_test_image_id;
	FontID m_test_font_id;

	void SetUp() override {
		m_test_image_id = m_resources.load_image("assets/image/render_test/test_image.png");
		ASSERT_NE(m_test_image_id, INVALID_IMAGE_ID) << "Failed to load test image!";

		m_test_font_id = m_resources.load_font("assets/font/ModernDOS8x16.ttf");
		ASSERT_NE(m_test_font_id, INVALID_FONT_ID) << "Failed to load test font!";
		m_resources.typeface(m_test_font_id).add_font(TEST_FONT_SIZE);
	}
};

TEST_F(RendererTests, ClearScreen) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	renderer.clear_screen(Color::turquoise());

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawPoint) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	for (int x = 0; x < BITMAP_WIDTH / 2; x++) {
		IVec2 position = { 2 * x, (uint8_t)(BITMAP_HEIGHT / 2 - 20) };
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, 255 } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, 255 } });
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawPoint_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	for (int x = 0; x < BITMAP_WIDTH / 2; x++) {
		IVec2 position = { 2 * x, (uint8_t)(BITMAP_HEIGHT / 2 - 20) };
		renderer.draw_point(Vertex { position + IVec2 { 0, 0 * 20 }, { 255, 0, 0, 127 } });
		renderer.draw_point(Vertex { position + IVec2 { 0, 1 * 20 }, { 0, 255, 0, 127 } });
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawLine_InterpolateColors) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(Vertex { pos1, Color::red() }, Vertex { pos2, Color::green() });
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawLine_InterpolateColors_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(Vertex { pos1, Color::red().with_alpha(0.5f) }, Vertex { pos2, Color::green().with_alpha(0.5f) });
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawLine_FixedColors) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(pos1, pos2, Color::green());
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawLine_FixedColors_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	const Vec2 positions[] = { { 0, 1 }, { M_SQRT1_2, M_SQRT1_2 }, { 1, 0 }, { M_SQRT1_2, -M_SQRT1_2 } };
	const int length = 150;
	for (Vec2 end : positions) {
		IVec2 pos1 = center + IVec2 { (int)(length / 2 * end.x), (int)(length / 2 * end.y) };
		IVec2 pos2 = center + IVec2 { -(int)(length / 2 * end.x), -(int)(length / 2 * end.y) };
		renderer.draw_line(pos1, pos2, Color::green().with_alpha(0.5f));
	}

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawRect) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect(rect, Color::green());

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawRect_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect(rect, Color::green().with_alpha(0.5f));

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawRectFill) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect_fill(rect, Color::green());

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawRectFill_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect rect = { BITMAP_WIDTH / 4, BITMAP_HEIGHT / 4, BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_rect_fill(rect, Color::green().with_alpha(0.5f));

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawCircle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle(center, 75, Color::green());

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawCircle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle(center, 75, Color::green().with_alpha(0.5f));

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawCircleFill) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle_fill(center, 75, Color::green());

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawCircleFill_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	renderer.draw_circle_fill(center, 75, Color::green().with_alpha(0.5f));

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawTriangle_EquilateralTriangle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, Color::red() };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, Color::green() };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, Color::blue() };
	renderer.draw_triangle(v1, v2, v3);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawTriangle_EquilateralTriangle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, Color::red().with_alpha(0.5f) };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, Color::green().with_alpha(0.5f) };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, Color::blue().with_alpha(0.5f) };
	renderer.draw_triangle(v1, v2, v3);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawTriangleFill_EquilateralTriangle) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, Color::red() };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, Color::green() };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, Color::blue() };
	renderer.draw_triangle_fill(v1, v2, v3);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawTriangleFill_EquilateralTriangle_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	int length = 75;
	Vertex v1 = { center + IVec2 { length * 0, length * -1 }, Color::red().with_alpha(0.5f) };
	Vertex v2 = { center + IVec2 { (int)(length * -M_SQRT3_2), (int)(length * 0.5f) }, Color::green().with_alpha(0.5f) };
	Vertex v3 = { center + IVec2 { (int)(length * M_SQRT3_2), (int)(length * 0.5f) }, Color::blue().with_alpha(0.5f) };
	renderer.draw_triangle_fill(v1, v2, v3);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	renderer.draw_image(m_test_image_id, center - image_size / 2);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	renderer.draw_image(m_test_image_id, center - image_size / 2, { .alpha = 0.5f });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_TintedRed) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	renderer.draw_image(m_test_image_id, center - image_size / 2, { .tint = Color { 255, 0, 0, 128 } });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_Flipped) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	renderer.draw_image(m_test_image_id, center - image_size / 2, { .flip_h = true, .flip_v = true });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_Clipped_TopLeft) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	Rect clip_rect = {
		.x = 0,
		.y = 0,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image(m_test_image_id, center - image_size / 4, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_Clipped_TopRight) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	Rect clip_rect = {
		.x = image.width / 2,
		.y = 0,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image(m_test_image_id, center - image_size / 4, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_Clipped_BottomLeft) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	Rect clip_rect = {
		.x = 0,
		.y = image.height / 2,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image(m_test_image_id, center - image_size / 4, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImage_Clipped_BottomRight) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 image_size = { image.width, image.height };
	Rect clip_rect = {
		.x = image.width / 2,
		.y = image.height / 2,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image(m_test_image_id, center - image_size / 4, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	renderer.draw_image_scaled(m_test_image_id, scaled_rect);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_HalfAlpha) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .alpha = 0.5f });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_TintedRed) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .tint = Color { 255, 0, 0, 127 } });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
TEST_F(RendererTests, DrawImageScaled_Flipped) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .flip_h = true, .flip_v = true });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_Clipped_TopLeft) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	Rect clip_rect = {
		.x = 0,
		.y = 0,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_Clipped_TopRight) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	Rect clip_rect = {
		.x = image.width / 2,
		.y = 0,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_Clipped_BottomLeft) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	Rect clip_rect = {
		.x = 0,
		.y = image.height / 2,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawImageScaled_Clipped_BottomRight) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	const Image& image = m_resources.image(m_test_image_id);
	int scale = 2;
	IVec2 center = { BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2 };
	IVec2 scaled_image_size = { scale * image.width, scale * image.height };
	Rect scaled_rect = Rect { center.x, center.y, scaled_image_size.x, scaled_image_size.y } - scaled_image_size / 2;
	Rect clip_rect = {
		.x = image.width / 2,
		.y = image.height / 2,
		.width = image.width / 2,
		.height = image.height / 2,
	};
	renderer.draw_image_scaled(m_test_image_id, scaled_rect, { .clip = clip_rect });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawFont_HorizontallyLeftAligned) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect text_rect = {
		.x = BITMAP_WIDTH / 8,
		.y = 0,
		.width = 3 * BITMAP_WIDTH / 4,
		.height = BITMAP_HEIGHT,
	};
	renderer.draw_text(m_test_font_id, TEST_FONT_SIZE, text_rect, Color::white(), LOREM_IPSUM, { .h_alignment = HorizontalAlignment::Left, .debug_draw_box = true });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawFont_HorizontallyCenterAligned) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect text_rect = {
		.x = BITMAP_WIDTH / 8,
		.y = 0,
		.width = 3 * BITMAP_WIDTH / 4,
		.height = BITMAP_HEIGHT,
	};
	renderer.draw_text(m_test_font_id, TEST_FONT_SIZE, text_rect, Color::white(), LOREM_IPSUM, { .h_alignment = HorizontalAlignment::Center, .debug_draw_box = true });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(RendererTests, DrawFont_HorizontallyRightAligned) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);

	Rect text_rect = {
		.x = BITMAP_WIDTH / 8,
		.y = 0,
		.width = 3 * BITMAP_WIDTH / 4,
		.height = BITMAP_HEIGHT,
	};
	renderer.draw_text(m_test_font_id, TEST_FONT_SIZE, text_rect, Color::white(), LOREM_IPSUM, { .h_alignment = HorizontalAlignment::Right, .debug_draw_box = true });

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
