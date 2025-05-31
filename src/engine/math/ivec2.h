#pragma once

#include <stdint.h>

namespace engine {

	struct IVec2 {
		int32_t x;
		int32_t y;

		IVec2& operator+=(const IVec2& rhs);
		IVec2& operator-=(const IVec2& rhs);
		IVec2& operator*=(const IVec2& rhs);
		IVec2& operator/=(const IVec2& rhs);

		friend IVec2 operator+(IVec2 lhs, const IVec2& rhs);
		friend IVec2 operator-(IVec2 lhs, const IVec2& rhs);
		friend IVec2 operator*(IVec2 lhs, const IVec2& rhs);
		friend IVec2 operator/(IVec2 lhs, const IVec2& rhs);
		friend IVec2 operator*(int lhs, IVec2 rhs);
		friend IVec2 operator/(IVec2 lhs, int rhs);
	};

} // namespace engine
