#include <engine/graphics/color.h>

#include <engine/math/math.h>

#include <cmath>

namespace engine {

	Color Color::lerp(Color start, Color end, float t) {
		return Color {
			.r = (uint8_t)engine::lerp(start.r, end.r, t),
			.g = (uint8_t)engine::lerp(start.g, end.g, t),
			.b = (uint8_t)engine::lerp(start.b, end.b, t),
			.a = (uint8_t)engine::lerp(start.a, end.a, t),
		};
	}

	Color Color::tint(Color color, Color tint) {
		return Color::lerp(color, color * tint, tint.a / 255.0f).with_alpha(color.a);
	}

	Color Color::with_alpha(uint8_t alpha) const {
		Color color = *this;
		color.a = alpha;
		return color;
	}

	Color Color::with_alpha(float alpha) const {
		Color color = *this;
		color.a = (uint8_t)(255.0f * alpha);
		return color;
	}

	// A fully transparent color can't be shown, and is thus "falsy"
	Color::operator bool() const {
		return this->a > 0;
	}

	Color operator+(const Color& lhs, const Color& rhs) {
		return Color {
			.r = (uint8_t)engine::min(lhs.r + rhs.r, 255),
			.g = (uint8_t)engine::min(lhs.g + rhs.g, 255),
			.b = (uint8_t)engine::min(lhs.b + rhs.b, 255),
			.a = (uint8_t)engine::min(lhs.a + rhs.a, 255),
		};
	}

	Color operator*(float t, const Color& rhs) {
		t = engine::clamp(t, 0.0f, 1.0f);
		return Color {
			.r = (uint8_t)std::round(t * rhs.r),
			.g = (uint8_t)std::round(t * rhs.g),
			.b = (uint8_t)std::round(t * rhs.b),
			.a = (uint8_t)std::round(t * rhs.a),
		};
	}

	Color operator*(const Color& lhs, float t) {
		return t * lhs;
	}

	Color operator*(Color lhs, const Color& rhs) {
		// Shortcut on identities
		if (rhs == Color::white()) {
			return lhs;
		}
		if (lhs == Color::white()) {
			return rhs;
		}

		// Note:
		// White should be the identity element for multiplication,
		// i.e. color * white = color, so we normalize before multipying, and then scale back to 0-255 range
		return Color {
			.r = (uint8_t)((255 * lhs.r * rhs.r) / (255 * 255)),
			.g = (uint8_t)((255 * lhs.g * rhs.g) / (255 * 255)),
			.b = (uint8_t)((255 * lhs.b * rhs.b) / (255 * 255)),
			.a = (uint8_t)((255 * lhs.a * rhs.a) / (255 * 255)),
		};
	}

} // namespace engine
