#pragma once

#include <engine/math/ivec2.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	class FontTestScreen {
	public:
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;
	};

} // namespace engine
