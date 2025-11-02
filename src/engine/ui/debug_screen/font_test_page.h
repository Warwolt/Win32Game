#pragma once

#include <engine/animation/animation_system_OLD.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct Input;
	class Renderer;

	class FontDebugPage {
	public:
		void initialize();
		void update(bool opened_now, const Input& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		AnimationSystem<int> m_animation_system;
	};

} // namespace engine
