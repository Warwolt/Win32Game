#pragma once

namespace engine {

	struct Vec2 {
		float x;
		float y;

		Vec2& operator+=(const Vec2& rhs);
		Vec2& operator-=(const Vec2& rhs);
		Vec2& operator*=(const Vec2& rhs);
		Vec2& operator/=(const Vec2& rhs);

		friend Vec2 operator+(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator-(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator*(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator/(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator*(float lhs, Vec2 rhs);
		friend Vec2 operator/(Vec2 lhs, float rhs);
	};

} // namespace engine
