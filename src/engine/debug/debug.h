#pragma once

#include <engine/debug/test/rendering_tests.h>

namespace engine {

	struct DebugState {
		struct TestScreens {
			RenderingTestScreen rendering;
		} test_screens;
	};

	void update_debug(DebugState* debug, const InputDevices& input);
	void draw_debug(Renderer* renderer, const DebugState& debug);

} // namespace engine
