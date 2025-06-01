#pragma once

#include <engine/math/ivec2.h>

namespace engine {

    struct InputDevices;
	class Renderer;

	class RendererTestScreen {
	public:
        void update(const InputDevices& input);
        void draw(Renderer* renderer) const;

	private:
		IVec2 m_window_size;
		uint8_t m_alpha = 255;
	};

} // namespace engine
