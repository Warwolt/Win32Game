#include <engine/graphics/font.h>

#include <fstream>

namespace engine {

	Font::Font(const Font& other) noexcept {
		m_file_data = other.m_file_data;
		stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
		m_font_data = other.m_font_data;
	}

	Font& Font::operator=(const Font& other) noexcept {
		m_file_data = other.m_file_data;
		stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
		m_font_data = other.m_font_data;
		return *this;
	}

	Font::Font(Font&& other) noexcept {
		if (this != &other) {
			m_file_data = std::move(other.m_file_data);
			stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
			other.m_font_info = {};
			m_font_data = std::move(other.m_font_data);
		}
	}

	Font& Font::operator=(Font&& other) noexcept {
		if (this != &other) {
			m_file_data = std::move(other.m_file_data);
			stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
			other.m_font_info = {};
			m_font_data = std::move(other.m_font_data);
		}
		return *this;
	}

	std::optional<Font> Font::from_path(std::filesystem::path path) {
		Font font;

		if (!std::filesystem::exists(path)) {
			return {};
		}

		/* Read ttf file */
		{
			size_t file_size = std::filesystem::file_size(path);
			std::ifstream font_file(path, std::ios_base::binary);
			font.m_file_data.resize(file_size);
			if (!font_file.read(reinterpret_cast<char*>(font.m_file_data.data()), file_size)) {
				return {};
			}
		}

		/* Prepare font */
		bool init_result = stbtt_InitFont(&font.m_font_info, font.m_file_data.data(), 0);
		if (!init_result) {
			return {};
		}

		return font;
	}

	Glyph& Font::glyph(int32_t size, char codepoint) {
		FontData& font = _get_or_make_font_data(size);
		if (auto it = font.glyphs.find(codepoint); it != font.glyphs.end()) {
			return it->second;
		}

		font.glyphs[codepoint] = _make_glyph(font, codepoint);
		return font.glyphs[codepoint];
	}

	int32_t Font::ascent(int32_t size) {
		FontData& font = _get_or_make_font_data(size);
		return font.ascent;
	}

	int32_t Font::text_width(int32_t size, const std::string& text) {
		int32_t text_width = 0;
		for (size_t i = 0; i < text.length(); i++) {
			const engine::Glyph& glyph = this->glyph(size, text[i]);
			if (i + 1 == text.length()) {
				text_width += glyph.width;
			}
			else {
				text_width += glyph.advance_width;
			}
		}
		return text_width;
	}

	Font::FontData& Font::_get_or_make_font_data(int32_t size) {
		if (auto it = m_font_data.find(size); it != m_font_data.end()) {
			return it->second;
		}

		float scale = stbtt_ScaleForPixelHeight(&m_font_info, (float)size);
		int ascent;
		stbtt_GetFontVMetrics(&m_font_info, &ascent, nullptr, nullptr);
		ascent = (int)std::round(ascent * scale);
		m_font_data[size] = FontData {
			.size = size,
			.ascent = ascent,
			.scale = scale,
			.glyphs = {},
		};
		return m_font_data[size];
	}

	Glyph Font::_make_glyph(const FontData& font, char codepoint) const {
		int advance_width, left_side_bearing;
		stbtt_GetCodepointHMetrics(&m_font_info, codepoint, &advance_width, &left_side_bearing);
		advance_width = (int)std::round(advance_width * font.scale);
		left_side_bearing = (int)std::round(left_side_bearing * font.scale);

		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&m_font_info, codepoint, font.scale, font.scale, &x0, &y0, &x1, &y1);
		int width = x1 - x0;
		int height = y1 - y0;

		std::vector<uint8_t> pixels(width * height);
		stbtt_MakeCodepointBitmap(&m_font_info, pixels.data(), width, height, width, font.scale, font.scale, codepoint);

		return Glyph {
			.width = width,
			.height = height,
			.y_offset = y0,
			.advance_width = advance_width,
			.left_side_bearing = left_side_bearing,
			.pixels = pixels,
		};
	}

} // namespace engine
