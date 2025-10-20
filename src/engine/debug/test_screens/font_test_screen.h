#pragma once

#include <engine/input/time.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	class FontTestScreen {
	public:
		void update(bool opened_now, const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		Time m_last_animation_frame;
		int m_animation_index = 0;
	};

} // namespace engine
