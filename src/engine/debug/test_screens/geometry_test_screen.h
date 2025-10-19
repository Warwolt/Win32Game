#pragma once

#include <engine/math/ivec2.h>

#include <stdint.h>
#include <vector>

namespace engine {

	struct InputDevices;
	class Renderer;

	class GeometryTestScreen {
	public:
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		uint8_t m_alpha = 255;
	};

} // namespace engine
