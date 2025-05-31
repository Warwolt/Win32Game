#include <engine/math/vec2.h>

namespace engine {

	Vec2& Vec2::operator+=(const Vec2& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

    Vec2& Vec2::operator-=(const Vec2& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

    Vec2& Vec2::operator*=(const Vec2& rhs) {
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}

    Vec2& Vec2::operator/=(const Vec2& rhs) {
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}

	Vec2 operator+(Vec2 lhs, const Vec2& rhs) {
		lhs += rhs;
		return lhs;
	}

    Vec2 operator-(Vec2 lhs, const Vec2& rhs) {
		lhs -= rhs;
		return lhs;
	}

    Vec2 operator*(Vec2 lhs, const Vec2& rhs) {
		lhs *= rhs;
		return lhs;
	}

    Vec2 operator/(Vec2 lhs, const Vec2& rhs) {
		lhs /= rhs;
		return lhs;
	}

    Vec2 operator*(float lhs, Vec2 rhs) {
		return Vec2 {
			.x = lhs * rhs.x,
			.y = lhs * rhs.y,
		};
	}

	Vec2 operator/(Vec2 lhs, float rhs) {
		return Vec2 {
			.x = lhs.x / rhs,
			.y = lhs.y / rhs,
		};
	}

} // namespace engine
