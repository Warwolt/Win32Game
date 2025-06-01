#pragma once

#include <stdint.h>

namespace engine {

	struct RGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a = 255;

		static RGBA lerp(RGBA start, RGBA end, float t);
	};


} // namespace engine
