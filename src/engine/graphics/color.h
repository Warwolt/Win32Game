#pragma once

#include <stdint.h>

namespace engine {

	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		static constexpr Color white() { return { 255, 255, 255, 255 }; }
		static constexpr Color grey() { return { 155, 173, 183, 255 }; }
		static constexpr Color black() { return { 0, 0, 0, 255 }; }
		static constexpr Color red() { return { 255, 0, 0, 255 }; }
		static constexpr Color green() { return { 0, 255, 0, 255 }; }
		static constexpr Color dark_green() { return { 0, 127, 0, 255 }; }
		static constexpr Color blue() { return { 0, 0, 255, 255 }; }
		static constexpr Color purple() { return { 255, 0, 255, 255 }; }
		static constexpr Color dark_purple() { return { 127, 0, 127, 255 }; }
		static constexpr Color yellow() { return { 251, 242, 54, 255 }; }
		static constexpr Color turquoise() { return { 0, 127, 127, 255 }; }

		static Color lerp(Color start, Color end, float t);
		static Color tint(Color color, Color tint);
		Color with_alpha(uint8_t alpha) const;
		Color with_alpha(float alpha) const;
		bool operator==(const Color& rhs) const = default;
		operator bool() const;
		friend Color operator+(const Color& lhs, const Color& rhs);
		friend Color operator*(const Color& lhs, float t);
		friend Color operator*(float t, const Color& rhs);
		friend Color operator*(Color lhs, const Color& rhs);
	};

} // namespace engine
