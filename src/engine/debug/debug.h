#pragma once

#include <engine/debug/test/rendering_test_screen.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct DebugState {
		struct TestScreens {
			RenderingTestScreen rendering;
		} test_screens;
	};

	void update_debug(DebugState* debug, const InputDevices& input);
	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution);

} // namespace engine
