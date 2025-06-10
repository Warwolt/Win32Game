#pragma once

#include <stdint.h>

namespace engine {

	// Integer valued rectangle
	// (x,y) is top left
	// width and height are measured in unit squares,
	// so a 1x1 rectangle is a single pixel big.
	struct Rect {
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;

		bool empty() const;
	};

} // namespace engine
