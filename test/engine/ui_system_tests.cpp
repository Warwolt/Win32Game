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

		Margin& with_value(int32_t value) {
			this->left = value;
			this->right = value;
			this->top = value;
			this->bottom = value;
			return *this;
		}

		int32_t horizontal() const {
			return left + right;
		}

		int32_t vertical() const {
			return top + bottom;
		}
	};

	struct Border {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
		Color color;

		Border& with_value(int32_t value) {
			this->left = value;
			this->right = value;
			this->top = value;
			this->bottom = value;
			return *this;
		}

		Border& with_color(Color value) {
			this->color = value;
			return *this;
		}

		int32_t horizontal() const {
			return left + right;
		}

		int32_t vertical() const {
			return top + bottom;
		}
	};

	struct Padding {
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;

		Padding& with_value(int32_t value) {
			this->left = value;
			this->right = value;
			this->top = value;
			this->bottom = value;
			return *this;
		}

		int32_t horizontal() const {
			return left + right;
		}

		int32_t vertical() const {
			return top + bottom;
		}
	};

	struct Box {
		IVec2 position;
		int32_t width;
		int32_t height;
		Color color;
		Padding padding;
		Border border;
		Margin margin;
	};

	struct Style {
		// box
		Margin margin;
		Border border;
		Padding padding;
		Color background_color;
		// text
		FontID font_id;
		int32_t font_size;
		Color font_color;
		Alignment text_alignment;
	};

	struct Element;

	// <p>
	struct TextContent {
		std::string text;
		FontID font_id;
		int32_t font_size;
		Color font_color;
		Alignment alignment;
	};

	// <img>
	struct ImageContent {
		ImageID image_id;
	};

	// <div>
	struct ContainerContent {
		std::vector<std::unique_ptr<Element>> children;
	};

	using Content = std::variant<TextContent, ImageContent, ContainerContent>;

	struct Element {
		Content content;
		Box box;
	};

	struct Document {
		std::vector<Element> root_elements;
	};

	// The application programmer interface
	class UISystem {
	public:
		void set_window_size(int32_t width, int32_t height);

		// void begin_box();
		// void end_box();
		void text(std::string text, Style = {});
		void image(ImageID image_id, Style = {});

		void begin_frame();
		void end_frame(const ResourceManager& resources);
		void draw(Renderer* renderer) const;

	private:
		void _build_layout(Document* document, const ResourceManager& resources) const;
		void _layout_element(Element* element, const ResourceManager& resources, int32_t available_width, int32_t available_height) const;
		void _draw_element(Renderer* renderer, IVec2* cursor, const Element& element) const;

		IVec2 m_window_size;
		Document m_document;
	};

	void UISystem::set_window_size(int32_t width, int32_t height) {
		m_window_size = { width, height };
	}

	void UISystem::text(std::string text, Style style) {
		m_document.root_elements.push_back(
			Element {
				.content = TextContent {
					.text = text,
					.font_id = style.font_id.value ? style.font_id : DEFAULT_FONT_ID,
					.font_size = style.font_size ? style.font_size : 16,
					.font_color = style.font_color ? style.font_color : Color::black(),
					.alignment = style.text_alignment },
				.box = {
					.position = { 0, 0 },
					.color = style.background_color,
					.padding = style.padding,
					.border = style.border,
					.margin = style.margin,
				},
			}
		);
	}

	void UISystem::image(ImageID image_id, Style style) {
		m_document.root_elements.push_back(
			Element {
				.content = ImageContent {
					.image_id = image_id,
				},
				.box = {
					.position = { 0, 0 },
					.color = style.background_color,
					.padding = style.padding,
					.border = style.border,
					.margin = style.margin,
				},
			}
		);
	}

	void UISystem::begin_frame() {
		// TODO handle input stuff here
	}

	void UISystem::end_frame(const ResourceManager& resources) {
		_build_layout(&m_document, resources);
	}

	void UISystem::draw(Renderer* renderer) const {
		IVec2 cursor = { 0, 0 };
		for (const Element& element : m_document.root_elements) {
			_draw_element(renderer, &cursor, element);
		}
	}

	void UISystem::_build_layout(Document* document, const ResourceManager& resources) const {
		for (Element& element : document->root_elements) {
			_layout_element(&element, resources, m_window_size.x, m_window_size.y);
		}
	}

	void UISystem::_layout_element(Element* element, const ResourceManager& resources, int32_t available_width, int32_t /*available_height*/) const {
		const Margin& margin = element->box.margin;
		const Border& border = element->box.border;
		const Padding& padding = element->box.padding;

		if (TextContent* content = std::get_if<TextContent>(&element->content)) {
			const Typeface& typeface = resources.typeface(content->font_id);
			const int32_t desired_content_width = available_width - margin.horizontal() - border.horizontal() - padding.horizontal();

			int32_t num_lines = 1;
			int32_t current_line_width = 0;
			for (char character : content->text) {
				current_line_width += typeface.glyph(content->font_size, character).advance_width;
				if (current_line_width > desired_content_width) {
					num_lines++;
					current_line_width = 0;
				}
			}

			element->box.width = desired_content_width;
			element->box.height = num_lines * typeface.line_height(content->font_size);
		}

		if (ImageContent* content = std::get_if<ImageContent>(&element->content)) {
			const Image& image = resources.image(content->image_id);
			element->box.width = image.width;
			element->box.height = image.height;
		}
	}

	void UISystem::_draw_element(Renderer* renderer, IVec2* cursor, const Element& element) const {
		const Margin& margin = element.box.margin;
		const Border& border = element.box.border;
		const Padding& padding = element.box.padding;

		Rect border_rect = {
			.x = cursor->x + margin.left,
			.y = cursor->y + margin.top,
			.width = element.box.width + padding.horizontal() + border.horizontal(),
			.height = element.box.height + padding.vertical() + border.vertical(),
		};

		Rect background_rect = {
			.x = border_rect.x + border.left,
			.y = border_rect.y + border.top,
			.width = border_rect.width - border.horizontal(),
			.height = border_rect.height - border.vertical(),
		};

		Rect content_rect = {
			.x = background_rect.x + padding.left,
			.y = background_rect.y + padding.top,
			.width = background_rect.width - padding.horizontal(),
			.height = background_rect.height - padding.vertical(),
		};

		/* Draw */
		renderer->draw_rect(border_rect, border.color); // border
		renderer->draw_rect_fill(background_rect, element.box.color); // background
		if (const TextContent* content = std::get_if<TextContent>(&element.content)) {
			content_rect.x += 1; // hack: add 1 pixel spacing between padding and text since it looks more correct
			renderer->draw_text(content->font_id, content->font_size, content_rect, content->font_color, content->text, { .alignment = content->alignment });
		}
		if (const ImageContent* content = std::get_if<ImageContent>(&element.content)) {
			renderer->draw_image_scaled(content->image_id, content_rect);
		}

		/* Update cursor */
		cursor->y += margin.vertical() + border.vertical() + padding.vertical() + element.box.height;
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
		m_resources = ResourceManager::initialize("assets/font/ModernDOS8x16.ttf").value();
		m_resources.typeface(DEFAULT_FONT_ID).add_font(TEST_FONT_SIZE);

		m_test_image_id = m_resources.load_image("assets/image/render_test/test_image.png");
		ASSERT_NE(m_test_image_id, INVALID_IMAGE_ID) << "Failed to load test image!";
	}
};

TEST_F(UISystemTests, TextElement_SingleLineParagraph) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
	};
	ui.text("The quick brown fox.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_SingleLineParagraph_WithPadding) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.padding = ui::Padding().with_value(10),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
	};
	ui.text("The quick brown fox.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_SingleLineParagraph_WithPadding_Centered) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.padding = ui::Padding().with_value(10),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
		.text_alignment = Alignment::Center,
	};
	ui.text("The quick brown fox.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_SingleLineParagraph_WithPadding_RightAligned) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.padding = ui::Padding().with_value(10),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
		.text_alignment = Alignment::Right,
	};
	ui.text("The quick brown fox.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_TwoSingleLineParagraphs) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
	};
	ui.text("The quick brown fox.", style);
	ui.text("Jumps over the lazy dog.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_MultilineParagraph) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
	};
	ui.text("The quick brown fox jumps over the lazy dog.", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, TextElement_TwoMultilineParagraphs) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui::Style style = {
		.margin = ui::Margin().with_value(1),
		.border = ui::Border().with_value(1).with_color(Color::black()),
		.background_color = Color::red(),
		.font_size = TEST_FONT_SIZE,
	};
	ui.text("The quick brown fox jumps over the lazy dog.", style);
	ui.text("Sphinx of black quarts, judge my vow!", style);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, ImageElement_SingleImage) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui.image(m_test_image_id);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, ImageElement_TwoImages_InColumn) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui.image(m_test_image_id);
	ui.image(m_test_image_id);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}

TEST_F(UISystemTests, ImageElement_TextElement) {
	Renderer renderer = Renderer::with_bitmap(BITMAP_WIDTH, BITMAP_HEIGHT);
	ui::UISystem ui = ui::UISystem();
	ui.set_window_size(BITMAP_WIDTH, BITMAP_HEIGHT);
	renderer.clear_screen(Color::white());

	ui.begin_frame();
	ui.text("First image:");
	ui.image(m_test_image_id, { .margin = { .bottom = 8 } });
	ui.text("Second image:");
	ui.image(m_test_image_id);
	ui.end_frame(m_resources);

	ui.draw(&renderer);
	renderer.render(m_resources);
	EXPECT_IMAGE_EQ_SNAPSHOT(renderer.bitmap().to_image());
}
