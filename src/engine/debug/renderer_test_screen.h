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
	};

} // namespace engine
