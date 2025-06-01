#pragma once

#include <engine/debug/test/rendering_test_screen.h>

namespace engine {

	struct DebugState {
		struct TestScreens {
			RenderingTestScreen rendering;
		} test_screens;
	};

	void update_debug(DebugState* debug, const InputDevices& input, const Window& window);
	void draw_debug(Renderer* renderer, const DebugState& debug);

} // namespace engine
