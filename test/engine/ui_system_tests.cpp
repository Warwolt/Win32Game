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
        RGBA color;
	};

	struct Padding {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
	};

	struct Box {
		IVec2 position;
        RGBA color;
		Padding padding;
		Border border;
		Margin margin;
	};

	struct Element;

	// <p>
	struct Text {
		std::string text;
		FontID font_id;
		int32_t font_size;
		RGBA font_color;
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

		void draw(Renderer* renderer) const;

	private:
		void _draw_element(Renderer* renderer) const;

		IVec2 m_window_size;
	};

	void UISystem::set_window_size(int32_t width, int32_t height) {
		m_window_size = { width, height };
	}

	void UISystem::text(std::string text) {
		//
	}

	void UISystem::draw(Renderer* renderer) const {
		_draw_element(renderer);
	}

	void UISystem::_draw_element(Renderer* renderer) const {
		Element element = {
			.content = Text {
				.text = "Hello world",
				.font_id = FontID(2),
				.font_size = 16,
				.font_color = RGBA::black(),
			},
			.box = {
				.position = { 0, 0 },
                .color = RGBA::red(),
				.padding = {
					.left = 1,
				},
				.border = {
					.left = 1,
					.right = 1,
					.top = 1,
					.bottom = 1,
                    .color = RGBA::black(),
				},
				.margin = {
					.left = 1,
					.right = 1,
					.top = 1,
					.bottom = 0,
				},
			}
		};

		Margin& margin = element.box.margin;
		Border& border = element.box.border;
		Padding& padding = element.box.padding;
		Text& content = std::get<Text>(element.content);

		int32_t box_width = m_window_size.x - margin.left - margin.right;
		int32_t box_height = content.font_size;

		Rect border_rect = {
			.x = margin.left,
			.y = margin.top,
			.width = box_width,
			.height = box_height,
		};

		Rect background_rect = {
			.x = border_rect.x + border.left,
			.y = border_rect.y + border.top,
			.width = border_rect.width - border.left - border.right,
			.height = border_rect.height - border.top - border.bottom,
		};

		Rect content_rect = {
			.x = background_rect.x + padding.left,
			.y = background_rect.y + padding.top,
			.width = background_rect.width - padding.left - padding.right,
			.height = background_rect.height - padding.top - padding.bottom,
		};

		/* Draw */
		renderer->draw_rect(border_rect, border.color); // border
		renderer->draw_rect_fill(background_rect, element.box.color); // background
		renderer->draw_text(content.font_id, content.font_size, content_rect, content.font_color, content.text); // content
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
