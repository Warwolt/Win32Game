#pragma once

#include <engine/math/ivec2.h>
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

		inline bool empty() const {
			return this->width == 0 && this->height == 0;
		}
		inline IVec2 pos() const {
			return IVec2 { this->x, this->y };
		}
	};

} // namespace engine
