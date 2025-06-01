#pragma once

#include <engine/debug/test/rendering_tests.h>

namespace engine {

	struct DebugState {
		struct TestScreens {
			RenderingTestScreen rendering;
		} test_screens;
	};

} // namespace engine
