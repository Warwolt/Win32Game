#include <engine/math/ivec2.h>

namespace engine {

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

} // namespace engine
