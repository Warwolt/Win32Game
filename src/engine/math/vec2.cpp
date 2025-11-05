#include <engine/math/math.h>
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

	float Vec2::length() const {
		return sqrtf((float)(this->x * this->x + this->y * this->y));
	}

	Vec2 Vec2::normalized() const {
		float length = this->length();
		if (length == 0) {
			return Vec2 { 0, 0 };
		}
		return Vec2 { this->x / length, this->y / length };
	}

	Vec2 Vec2::lerp(Vec2 lhs, const Vec2& rhs, float t) {
		return Vec2 {
			.x = engine::lerp(lhs.x, rhs.x, t),
			.y = engine::lerp(lhs.y, rhs.y, t),
		};
	}

} // namespace engine
