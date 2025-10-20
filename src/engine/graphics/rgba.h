#pragma once

#include <stdint.h>

namespace engine {

	struct RGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		static constexpr RGBA white() { return { 255, 255, 255, 255 }; }
		static constexpr RGBA black() { return { 0, 0, 0, 255 }; }
		static constexpr RGBA red() { return { 255, 0, 0, 255 }; }
		static constexpr RGBA green() { return { 0, 255, 0, 255 }; }
		static constexpr RGBA blue() { return { 0, 0, 255, 255 }; }
		static constexpr RGBA purple() { return { 255, 0, 255, 255 }; }
		static constexpr RGBA dark_purple() { return { 127, 0, 127, 255 }; }

		static RGBA lerp(RGBA start, RGBA end, float t);
		static RGBA tint(RGBA color, RGBA tint);
		RGBA with_alpha(uint8_t alpha) const;
		RGBA with_alpha(float alpha) const;
		bool operator==(const RGBA& rhs) const = default;
		operator bool() const;
		friend RGBA operator+(const RGBA& lhs, const RGBA& rhs);
		friend RGBA operator*(const RGBA& lhs, float t);
		friend RGBA operator*(float t, const RGBA& rhs);
		friend RGBA operator*(RGBA lhs, const RGBA& rhs);
	};

} // namespace engine
