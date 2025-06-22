#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test/rendering_test_screen.h>
#include <engine/graphics/font_id.h>
#include <engine/math/ivec2.h>
#include <engine/commands.h>

namespace engine {

	class ResourceManager;

	struct DebugState {
		FontID debug_font_id;
		bool show_cpu_timing_overlay = false;
		bool menu_bar_focused = false;
		bool file_menu_opened = false;

		struct CPUPerformance {
			DeltaTimer input_timer;
			DeltaTimer update_timer;
			DeltaTimer draw_timer;
			DeltaTimer render_timer;
			DeltaTimer frame_timer;
		} performance;

		bool show_rendering_test_screen = false;
		RenderingTestScreen rendering_test_screen;
	};

	void initialize_debug(DebugState* debug, ResourceManager* resources);
	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands);
	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution);

} // namespace engine
