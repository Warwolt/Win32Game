#pragma once

#include <engine/math/vec2.h>

#include <stdint.h>

namespace engine {

	struct IVec2 {
		int32_t x;
		int32_t y;

		static IVec2 from_vec2(Vec2 vec2);

		bool operator==(const IVec2& rhs) const = default;
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
