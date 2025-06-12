#pragma once

#include <engine/math/vec2.h>

#include <functional>
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

template <>
struct std::hash<engine::IVec2> {
	size_t operator()(const engine::IVec2& vec) const noexcept {
		size_t h1 = std::hash<int32_t>()(vec.x);
		size_t h2 = std::hash<int32_t>()(vec.y);
		return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // boost::hash_combine
	}
};
