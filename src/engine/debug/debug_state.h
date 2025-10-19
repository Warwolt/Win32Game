#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test_screens/test_screens.h>
#include <engine/graphics/font_id.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct DebugState {
		FontID debug_font_id;
		bool show_test_screens = true;
		TestScreens test_screens;
		DeltaTimer frame_timer;
	};

} // namespace engine
