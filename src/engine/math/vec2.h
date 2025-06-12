#pragma once

#include <functional>

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

		Vec2 lerp(const Vec2& rhs, float t);
	};

} // namespace engine

template <>
struct std::hash<engine::Vec2> {
	size_t operator()(const engine::Vec2& vec) const noexcept {
		size_t h1 = std::hash<float>()(vec.x);
		size_t h2 = std::hash<float>()(vec.y);
		return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // boost::hash_combine
	}
};
