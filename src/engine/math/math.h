#pragma once

namespace engine {

	inline float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}

	template <class T>
	inline const T& min(const T& a, const T& b) {
		return (b < a) ? b : a;
	}

	template <class T>
	inline const T& max(const T& a, const T& b) {
		return (a < b) ? b : a;
	}

	template <class T>
	inline const T& clamp(const T& x, const T& min, const T& max) {
		return engine::min(engine::max(x, min), max);
	}

} // namespace engine
