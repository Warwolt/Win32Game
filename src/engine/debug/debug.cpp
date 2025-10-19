#include <engine/debug/assert.h>
#include <engine/debug/debug.h>
#include <engine/debug/debug_state.h>
#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

#include <optional>

namespace engine {

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->render_test_screen.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands) {
		CPUProfilingScope_Engine();

		/* Update test screens */
		if (input.keyboard.key_was_pressed_now(VK_F3)) {
			debug->show_render_test_screen = !debug->show_render_test_screen;
		}
		if (debug->show_render_test_screen) {
			debug->render_test_screen.update(input);
		}

		/* Show CPU profiling information in window title */
		float avg_fps = 1.0f / debug->frame_timer.average_delta();
		const char* window_title = "Game";
		std::string window_title_with_fps = std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->push_back(AppCommand_SetWindowTitle { window_title_with_fps });
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution) {
		CPUProfilingScope_Engine();

		/* Draw test screens */
		if (debug.show_render_test_screen) {
			debug.render_test_screen.draw(renderer, screen_resolution);
		}
	}

} // namespace engine
