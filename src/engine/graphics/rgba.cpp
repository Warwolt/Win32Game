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

	RGBA operator+(const RGBA& lhs, const RGBA& rhs) {
		return RGBA {
			.r = (uint8_t)engine::clamp(lhs.r + rhs.r, 0, 255),
			.g = (uint8_t)engine::clamp(lhs.g + rhs.g, 0, 255),
			.b = (uint8_t)engine::clamp(lhs.b + rhs.b, 0, 255),
			.a = (uint8_t)engine::clamp(lhs.a + rhs.a, 0, 255),
		};
	}

	RGBA operator*(float t, const RGBA& rhs) {
		return RGBA {
			.r = (uint8_t)std::round(engine::clamp(t * rhs.r, 0.0f, 255.0f)),
			.g = (uint8_t)std::round(engine::clamp(t * rhs.g, 0.0f, 255.0f)),
			.b = (uint8_t)std::round(engine::clamp(t * rhs.b, 0.0f, 255.0f)),
			.a = (uint8_t)std::round(engine::clamp(t * rhs.a, 0.0f, 255.0f)),
		};
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
