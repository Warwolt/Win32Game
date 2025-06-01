#pragma once

#include <engine/math/ivec2.h>

namespace engine {

    struct InputDevices;
	class Window;
	class Renderer;

	class RenderingTestScreen {
	public:
		void update(const InputDevices& input);
        void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		uint8_t m_alpha = 255;
	};

} // namespace engine
