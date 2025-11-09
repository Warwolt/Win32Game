#include <engine/math/ivec2.h>

#include <engine/math/math.h>

#include <cmath>

namespace engine {

	IVec2 IVec2::from_vec2(Vec2 vec2) {
		return IVec2 {
			.x = (int32_t)std::round(vec2.x),
			.y = (int32_t)std::round(vec2.y),
		};
	}

	IVec2& IVec2::operator+=(const IVec2& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	IVec2& IVec2::operator-=(const IVec2& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	IVec2& IVec2::operator*=(const IVec2& rhs) {
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}

	IVec2& IVec2::operator/=(const IVec2& rhs) {
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}

	IVec2 operator+(IVec2 lhs, const IVec2& rhs) {
		lhs += rhs;
		return lhs;
	}

	IVec2 operator-(IVec2 lhs, const IVec2& rhs) {
		lhs -= rhs;
		return lhs;
	}

	IVec2 operator*(IVec2 lhs, const IVec2& rhs) {
		lhs *= rhs;
		return lhs;
	}

	IVec2 operator/(IVec2 lhs, const IVec2& rhs) {
		lhs /= rhs;
		return lhs;
	}

	IVec2 operator*(int lhs, IVec2 rhs) {
		return IVec2 {
			.x = lhs * rhs.x,
			.y = lhs * rhs.y,
		};
	}

	IVec2 operator/(IVec2 lhs, int rhs) {
		return IVec2 {
			.x = lhs.x / rhs,
			.y = lhs.y / rhs,
		};
	}

	float IVec2::length() const {
		return sqrtf((float)(this->x * this->x + this->y * this->y));
	}

	Vec2 IVec2::normalized() const {
		float length = this->length();
		return Vec2 { this->x / length, this->y / length };
	}

	bool IVec2::is_zero() const {
		return this->x == 0 && this->y == 0;
	}

	IVec2 IVec2::lerp(IVec2 lhs, const IVec2& rhs, float t) {
		return IVec2 {
			.x = (int32_t)std::round(engine::lerp((float)lhs.x, (float)rhs.x, t)),
			.y = (int32_t)std::round(engine::lerp((float)lhs.y, (float)rhs.y, t)),
		};
	}

} // namespace engine
