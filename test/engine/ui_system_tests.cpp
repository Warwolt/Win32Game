#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/font_id.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/renderer.h>
#include <engine/math/ivec2.h>

#include <optional>
#include <variant>

using namespace engine;

namespace engine::ui {

	struct Margin {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
	};

	struct Border {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
	};

	struct Padding {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
	};

	struct Box {
		IVec2 position;
		Padding padding;
		Border border;
		Margin margin;
	};

	struct Element;

	// <p>
	struct Text {
		std::string text;
		FontID font_id;
	};

	// <img>
	struct Image {
		ImageID image_id;
	};

	// <div>
	struct Container {
		std::vector<std::unique_ptr<Element>> children;
	};

	using Content = std::variant<Text, Image, Container>;

	struct Element {
		Content content;
		Box box;
	};

	struct Document {
		// tree of elements
	};

	// The application programmer interface
	class UISystem {
	public:
		void set_window_size(int32_t width, int32_t height);

		// void begin_frame();
		// void end_frame();

		// void begin_box();
		// void end_box();
		void text(std::string text);
		// void image();

		void draw(Renderer* renderer);

	private:
		// _layout_pass();

		IVec2 m_window_size;
	};

	void UISystem::set_window_size(int32_t width, int32_t height) {
		m_window_size = { width, height };
	}

	void UISystem::text(std::string text) {
		//
	}

	void UISystem::draw(Renderer* renderer) {
		FontID font_id = FontID(2);
		int32_t font_size = 16;
		int32_t margin_top = font_size;
		int32_t border_left = 1;
		int32_t border_right = 1;
		int32_t border_top = 1;
		int32_t border_bottom = 1;

		IVec2 position = { 0, 0 };
		int32_t box_width = m_window_size.x;
        int32_t box_height = font_size;

		/* Draw border */
		renderer->draw_rect({ position.x, position.y, box_width, box_height }, RGBA::black());

		/* Draw background */
		renderer->draw_rect_fill({ position.x + border_left, position.y + border_top, box_width - border_left - border_right, box_height - border_top - border_bottom }, RGBA::red());

		/* Draw content */
		renderer->draw_text(font_id, 16, { position.x + border_left, position.y + border_top}, RGBA::black(), "Hello world");
	}

} // namespace engine::ui

constexpr int BITMAP_WIDTH = 256;
constexpr int BITMAP_HEIGHT = 240;
constexpr int TEST_FONT_SIZE = 16;

class UISystemTests : public testing::Test {
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

TEST_F(UISystemTests, TextElement_TopLeft_RedBackground_BlackBorder) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);

	renderer.clear_screen(RGBA::white());
	ui.text("Hello world");
	ui.draw(&renderer);

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
