#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test/rendering_test_screen.h>
#include <engine/math/ivec2.h>

namespace engine {

	class ResourceManager;
	class Window;

	struct DebugState {
		struct CPUPerformance {
			DeltaTimer input_timer;
			DeltaTimer update_timer;
			DeltaTimer render_timer;
			DeltaTimer frame_timer;
		} performance;
		struct TestScreens {
			RenderingTestScreen rendering;
		} test_screens;
	};

	void initialize_debug(DebugState* debug, ResourceManager* resources);
	void update_debug(DebugState* debug, const InputDevices& input, Window* window, ResourceManager* resources);
	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution);

} // namespace engine
