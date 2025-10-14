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
		bool menu_bar_active; // we're navigating the menu bar
		bool menu_bar_file_item_focused = false; // white border
		bool file_menu_open; // controls "file" gets highlighted
		bool file_menu_exit_item_focused = false; // white border
		bool show_rendering_test_screen = false;
		RenderingTestScreen rendering_test_screen;
		DeltaTimer frame_timer;
	};

	void initialize_debug(DebugState* debug, ResourceManager* resources);
	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands);
	void draw_debug(Renderer* renderer, ResourceManager* resources, const DebugState& debug, IVec2 screen_resolution);

} // namespace engine
