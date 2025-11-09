#pragma once

#include <engine/animation/animation.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct Input;
	class Renderer;

	class FontDebugPage {
	public:
		void update(bool opened_now, const Input& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		bool m_render_narrow = false;
	};

} // namespace engine
