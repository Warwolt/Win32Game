#pragma once

#include <engine/debug/delta_timer.h>
#include <engine/debug/test/render_test_screen.h>
#include <engine/graphics/font_id.h>
#include <engine/math/ivec2.h>
#include <engine/commands.h>

namespace engine {

	class ResourceManager;

	struct DebugState {
		FontID debug_font_id;
		bool show_render_test_screen = true;
		RenderTestScreen render_test_screen;
		DeltaTimer frame_timer;
	};

	void initialize_debug(DebugState* debug, ResourceManager* resources);
	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands);
	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution);

} // namespace engine
