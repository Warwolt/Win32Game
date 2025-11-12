#pragma once

#include <engine/graphics/font.h>

#include <stb_truetype/stb_truetype.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

struct stbtt_fontinfo;

namespace engine {

	struct Glyph {
		int32_t width; // bitmap width
		int32_t height; // bitmap height
		int32_t y_offset; // distance from glyph origin to bitmap top
		int32_t advance_width; // space to insert between this glyph and next
		int32_t left_side_bearing; // distance from horisontal position to glyph
		std::vector<uint8_t> pixels; // bitmap data

		inline uint8_t get(int32_t x, int32_t y) const {
			if (0 <= x && x <= this->width) {
				if (0 <= y && y <= this->height) {
					return this->pixels[x + y * this->width];
				}
			}
			return 0;
		}
	};

	// FIXME: rename this to `Typeface`, since it collects various sizes, a typeface at specific size is a font
	class Font {
	public:
		Font() = default;
		Font(const Font& other) noexcept;
		Font& operator=(const Font& other) noexcept;
		Font(Font&& other) noexcept;
		Font& operator=(Font&& other) noexcept;

		static std::optional<Font> from_path(std::filesystem::path path);
		void add_font(int32_t size);
		const Glyph& glyph(int32_t size, char codepoint) const;
		int32_t ascent(int32_t size) const;
		int32_t text_width(int32_t size, const std::string& text) const;

	private:
		struct FontData {
			int32_t size;
			int32_t ascent;
			float scale;
			std::unordered_map<char, Glyph> glyphs;
		};

		const FontData& _get_font_data(int32_t size) const;
		Glyph _make_glyph(float font_scale, char codepoint) const;

		std::vector<uint8_t> m_file_data;
		stbtt_fontinfo m_font_info = {};
		std::unordered_map<int32_t, FontData> m_font_data;
	};

} // namespace engine
