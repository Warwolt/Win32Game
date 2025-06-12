#pragma once

#include <stdint.h>

namespace engine {

	struct RGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a = 255;

		static RGBA lerp(RGBA start, RGBA end, float t);
		bool operator==(const RGBA& rhs) const = default;
		friend RGBA operator+(const RGBA& lhs, const RGBA& rhs);
		friend RGBA operator*(float t, const RGBA& rhs);
	};

} // namespace engine
