#pragma once

#include <engine/math/ivec2.h>

#include <stdint.h>

namespace engine {

	// Integer valued rectangle
	//
	// (x,y) is top left. Width and height are measured in pixels, so a 1x1
	// rectangle is a single pixel big.
	struct Rect {
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;

		friend Rect operator+(Rect lhs, IVec2 rhs); // translate position
		friend Rect operator*(Rect lhs, float rhs); // scale relative top left corner

		bool empty() const;
		IVec2 pos() const;
	};

} // namespace engine
