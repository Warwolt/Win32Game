#include <engine/graphics/rgba.h>

#include <engine/math/math.h>

#include <cmath>

namespace engine {

	RGBA RGBA::lerp(RGBA start, RGBA end, float t) {
		return RGBA {
			.r = (uint8_t)engine::lerp(start.r, end.r, t),
			.g = (uint8_t)engine::lerp(start.g, end.g, t),
			.b = (uint8_t)engine::lerp(start.b, end.b, t),
			.a = (uint8_t)engine::lerp(start.a, end.a, t),
		};
	}

	RGBA RGBA::tint(RGBA color, RGBA tint) {
		return RGBA::lerp(color, color * tint, tint.a / 255.0f).with_alpha(color.a);
	}

	RGBA RGBA::with_alpha(uint8_t alpha) const {
		RGBA color = *this;
		color.a = alpha;
		return color;
	}

	RGBA RGBA::with_alpha(float alpha) const {
		RGBA color = *this;
		color.a = (uint8_t)(255.0f * alpha);
		return color;
	}

	// A fully transparent color can't be shown, and is thus "falsy"
	RGBA::operator bool() const {
		return this->a > 0;
	}

	RGBA operator+(const RGBA& lhs, const RGBA& rhs) {
		return RGBA {
			.r = (uint8_t)engine::min(lhs.r + rhs.r, 255),
			.g = (uint8_t)engine::min(lhs.g + rhs.g, 255),
			.b = (uint8_t)engine::min(lhs.b + rhs.b, 255),
			.a = (uint8_t)engine::min(lhs.a + rhs.a, 255),
		};
	}

	RGBA operator*(float t, const RGBA& rhs) {
		t = engine::clamp(t, 0.0f, 1.0f);
		return RGBA {
			.r = (uint8_t)std::round(t * rhs.r),
			.g = (uint8_t)std::round(t * rhs.g),
			.b = (uint8_t)std::round(t * rhs.b),
			.a = (uint8_t)std::round(t * rhs.a),
		};
	}

	RGBA operator*(const RGBA& lhs, float t) {
		return t * lhs;
	}

	RGBA operator*(RGBA lhs, const RGBA& rhs) {
		// Shortcut on identities
		if (rhs == RGBA::white()) {
			return lhs;
		}
		if (lhs == RGBA::white()) {
			return rhs;
		}

		// Note:
		// White should be the identity element for multiplication,
		// i.e. color * white = color, so we normalize before multipying, and then scale back to 0-255 range
		return RGBA {
			.r = (uint8_t)((255 * lhs.r * rhs.r) / (255 * 255)),
			.g = (uint8_t)((255 * lhs.g * rhs.g) / (255 * 255)),
			.b = (uint8_t)((255 * lhs.b * rhs.b) / (255 * 255)),
			.a = (uint8_t)((255 * lhs.a * rhs.a) / (255 * 255)),
		};
	}

} // namespace engine
