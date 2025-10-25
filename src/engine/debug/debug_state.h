#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test_screen/test_screen.h>
#include <engine/graphics/font_id.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct DebugState {
		bool show_test_screen = true;
		TestScreen test_screen;
		DeltaTimer frame_timer;
	};

} // namespace engine
