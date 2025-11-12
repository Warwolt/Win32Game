#include <engine/graphics/font.h>

#include <engine/debug/assert.h>

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
		Typeface font;

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

	void Typeface::add_font(int32_t size) {
		float scale = stbtt_ScaleForPixelHeight(&m_font_info, (float)size);
		int ascent;
		stbtt_GetFontVMetrics(&m_font_info, &ascent, nullptr, nullptr);
		ascent = (int)std::round(ascent * scale);
		std::unordered_map<char, Glyph> glyphs;
		for (char codepoint = ' '; codepoint < '~'; codepoint++) {
			glyphs[codepoint] = _make_glyph(scale, codepoint);
		}
		m_fonts[size] = {
			.size = size,
			.ascent = ascent,
			.scale = scale,
			.glyphs = std::move(glyphs),
		};
	}

	const Glyph& Typeface::glyph(int32_t size, char codepoint) const {
		const Font& font = _get_font(size);
		return font.glyphs.at(codepoint);
	}

	int32_t Typeface::ascent(int32_t size) const {
		const Font& font = _get_font(size);
		return font.ascent;
	}

	int32_t Typeface::text_width(int32_t size, const std::string& text) const {
		int32_t text_width = 0;
		for (size_t i = 0; i < text.length(); i++) {
			const engine::Glyph& glyph = this->glyph(size, text[i]);
			text_width += glyph.advance_width;
		}
		return text_width;
	}

	const Typeface::Font& Typeface::_get_font(int32_t size) const {
		auto it = m_fonts.find(size);
		DEBUG_ASSERT(it != m_fonts.end(), "Couldn't find typeface with size %d. Did you call `Typeface::add_font`?", size);
		return it->second;
	}

	Glyph Typeface::_make_glyph(float font_scale, char codepoint) const {
		int advance_width, left_side_bearing;
		stbtt_GetCodepointHMetrics(&m_font_info, codepoint, &advance_width, &left_side_bearing);
		advance_width = (int)std::round(advance_width * font_scale);
		left_side_bearing = (int)std::round(left_side_bearing * font_scale);

		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&m_font_info, codepoint, font_scale, font_scale, &x0, &y0, &x1, &y1);
		int width = x1 - x0;
		int height = y1 - y0;

		std::vector<uint8_t> pixels(width * height);
		stbtt_MakeCodepointBitmap(&m_font_info, pixels.data(), width, height, width, font_scale, font_scale, codepoint);

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
