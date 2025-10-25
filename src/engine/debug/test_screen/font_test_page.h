#pragma once

#include <engine/input/time.h>
#include <engine/math/ivec2.h>
#include <engine/animation/animation_system.h>

namespace engine {

	struct Input;
	class Renderer;

	class FontTestScreen {
	public:
		void initialize();
		void update(bool opened_now, const Input& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		AnimationSystem<int> m_animation_system;
	};

} // namespace engine
