#include <engine/graphics/font.h>

#include <fstream>

namespace engine {

	std::optional<Typeface> Typeface::from_path(std::filesystem::path path) {
		Typeface typeface;

		/* Read ttf file */
		{
			size_t file_size = std::filesystem::file_size(path);
			typeface.m_font_data.reserve(file_size);
			std::ifstream font_file(path, std::ios_base::binary);
			font_file.read((char*)typeface.m_font_data.data(), file_size);
			font_file.close();
		}

		/* Prepare typeface */
		bool init_result = stbtt_InitFont(&typeface.m_font_info, typeface.m_font_data.data(), 0);
		if (!init_result) {
			return {};
		}

		return typeface;
	}

	Glyph& Typeface::glyph(int32_t size, char codepoint) {
		Font& font = _get_or_make_font(size);
		if (auto it = font.glyphs.find(codepoint); it != font.glyphs.end()) {
			return it->second;
		}

		font.glyphs[codepoint] = _make_glyph(font, codepoint);
		return font.glyphs[codepoint];
	}

	Font& Typeface::_get_or_make_font(int32_t size) {
		if (auto it = m_fonts.find(size); it != m_fonts.end()) {
			return it->second;
		}

		float scale = stbtt_ScaleForPixelHeight(&m_font_info, (float)size);
		int ascent;
		stbtt_GetFontVMetrics(&m_font_info, &ascent, nullptr, nullptr);
		ascent = (int)std::round(ascent * scale);
		m_fonts[size] = Font {
			.size = size,
			.ascent = ascent,
			.scale = scale,
			.glyphs = {},
		};
		return m_fonts[size];
	}

	Glyph Typeface::_make_glyph(const Font& font, char codepoint) const {
		int advance_width;
		stbtt_GetCodepointHMetrics(&m_font_info, codepoint, &advance_width, nullptr);
		advance_width = (int)std::round(advance_width * font.scale);

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
			.pixels = pixels,
		};
	}

} // namespace engine
