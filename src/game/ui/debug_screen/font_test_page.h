#pragma once

#include <engine/animation/animation.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>

namespace engine {
	struct Input;
	class Renderer;
}

namespace game {

	class FontDebugPage {
	public:
		void update(bool opened_now, const engine::Input& input);
		void draw(engine::Renderer* renderer, engine::IVec2 screen_resolution) const;

	private:
		bool m_render_narrow = false;
	};

} // namespace game
