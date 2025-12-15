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
		uint32_t left;
		uint32_t right;
		uint32_t top;
		uint32_t bottom;
	};

	struct Border {
		uint32_t left;
		uint32_t right;
		uint32_t top;
		uint32_t bottom;
	};

	struct Padding {
		uint32_t left;
		uint32_t right;
		uint32_t top;
		uint32_t bottom;
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
		// void begin_frame();
		// void end_frame();

		// void begin_box();
		// void end_box();
		void text(std::string text);
		// void image();

		// draw_ui(Renderer* renderer);

	private:
		// _layout_pass();
	};

	void UISystem::text(std::string text) {
		//
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

TEST_F(UISystemTests, TextElement_TopLeft) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();

	renderer.clear_screen(RGBA::white());
	ui.text("Hello world");

	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
