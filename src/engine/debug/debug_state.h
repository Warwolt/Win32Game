#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test/render_test_screen.h>
#include <engine/graphics/font_id.h>
#include <engine/math/ivec2.h>

namespace engine {

	struct DebugState {
		FontID debug_font_id;
		bool show_render_test_screen = true;
		RenderTestScreen render_test_screen;
		DeltaTimer frame_timer;
	};

} // namespace engine
