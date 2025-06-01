#include <engine/graphics/rgba.h>

#include <cmath>

namespace engine {

	RGBA RGBA::lerp(RGBA start, RGBA end, float t) {
		return RGBA {
			.r = (uint8_t)std::lerp(start.r, end.r, t),
			.g = (uint8_t)std::lerp(start.g, end.g, t),
			.b = (uint8_t)std::lerp(start.b, end.b, t),
			.a = (uint8_t)std::lerp(start.a, end.a, t),
		};
	}

} // namespace engine
