#pragma once

#include <engine/animation/animation.h>
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
		AnimationLibrary<int> m_animation_library;
		AnimationPlayer<int> m_animation_player;
		AnimationID m_animation_id;
	};

} // namespace engine
