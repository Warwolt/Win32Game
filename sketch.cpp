namespace ui {

	struct Content {
		uint32_t width;
		uint32_t height;
	};

	struct Padding {
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

	struct Margin {
		uint32_t left;
		uint32_t right;
		uint32_t top;
		uint32_t bottom;
	};

	// +-----------------------------------+
	// |               margin              |
	// |   +---------------------------+   |
	// |   |           border          |   |
	// |   |   +-------------------+   |   |
	// |   |   |      padding      |   |   |
	// |   |   |   +-----------+   |   |   |
	// |   |   |   |  content  |   |   |   |
	// |   |   |   +-----------+   |   |   |
	// |   |   |                   |   |   |
	// |   |   +-------------------+   |   |
	// |   |                           |   |
	// |   +---------------------------+   |
	// |                                   |
	// +-----------------------------------+
	struct LayoutBox {
		IVec2 position;
		Content content;
		Padding padding;
		Border border;
		Margin margin;

		IVec2 size() const {
			return { width(), height() };
		}

		uint32_t width() const {
			return content.width + padding.left + padding.right + border.left + border.right;
		}

		uint32_t height() const {
			return content.height + padding.top + padding.bottom + border.top + border.bottom;
		}
	};

	// 1. Declare UI elements (text + images + boxes) with desired layout (position + size)
	// 2. Compute actual layout (position + sizes)
	// 3. Draw elements according to layout

}
