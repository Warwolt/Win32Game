#pragma once

#include <engine/graphics/font.h>

#include <stb_truetype/stb_truetype.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

struct stbtt_fontinfo;

namespace engine {

	struct Glyph {
		int32_t width;               // bitmap width
		int32_t height;              // bitmap height
		int32_t y_offset;            // distance from glyph origin to bitmap top
		int advance_width;           // space to insert between this glyph and next
		std::vector<uint8_t> pixels; // bitmap data

		inline uint8_t get(int32_t x, int32_t y) {
			if (0 <= x && x <= this->width) {
				if (0 <= y && y <= this->height) {
					return this->pixels[x + y * this->width];
				}
			}
			return 0;
		}
	};

	struct Font {
		int32_t size;
		int32_t ascent;
		float scale;
		std::unordered_map<char, Glyph> glyphs;
	};

	class Typeface {
	public:
		static std::optional<Typeface> from_path(std::filesystem::path path);
		Glyph& glyph(int32_t size, char codepoint);

	private:
		Font& _get_or_make_font(int32_t size);
		Glyph _make_glyph(const Font& font, char codepoint) const;

		std::vector<uint8_t> m_file_data;
		stbtt_fontinfo m_font_info;
		std::unordered_map<int32_t, Font> m_fonts;
	};

} // namespace engine
