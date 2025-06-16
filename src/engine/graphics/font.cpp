#include <engine/graphics/font.h>

#include <fstream>

namespace engine {

	Typeface::Typeface(const Typeface& other) noexcept {
		m_file_data = other.m_file_data;
		stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
		m_fonts = other.m_fonts;
	}

	Typeface& Typeface::operator=(const Typeface& other) noexcept {
		m_file_data = other.m_file_data;
		stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
		m_fonts = other.m_fonts;
		return *this;
	}

	Typeface::Typeface(Typeface&& other) noexcept {
		if (this != &other) {
			m_file_data = std::move(other.m_file_data);
			stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
			other.m_font_info = {};
			m_fonts = std::move(other.m_fonts);
		}
	}

	Typeface& Typeface::operator=(Typeface&& other) noexcept {
		if (this != &other) {
			m_file_data = std::move(other.m_file_data);
			stbtt_InitFont(&m_font_info, m_file_data.data(), 0);
			other.m_font_info = {};
			m_fonts = std::move(other.m_fonts);
		}
		return *this;
	}

	std::optional<Typeface> Typeface::from_path(std::filesystem::path path) {
		Typeface typeface;

		if (!std::filesystem::exists(path)) {
			return {};
		}

		/* Read ttf file */
		{
			size_t file_size = std::filesystem::file_size(path);
			std::ifstream font_file(path, std::ios_base::binary);
			typeface.m_file_data.resize(file_size);
			if (!font_file.read(reinterpret_cast<char*>(typeface.m_file_data.data()), file_size)) {
				return {};
			}
		}

		/* Prepare typeface */
		bool init_result = stbtt_InitFont(&typeface.m_font_info, typeface.m_file_data.data(), 0);
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
