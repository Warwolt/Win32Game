#pragma once

#include <engine/math/ivec2.h>

namespace engine {

    struct InputDevices;
	struct Window;
	class Renderer;

	class RenderingTestScreen {
	public:
		void update(const InputDevices& input, const Window& window);
        void draw(Renderer* renderer) const;

	private:
		IVec2 m_window_size;
		uint8_t m_alpha = 255;
	};

} // namespace engine
