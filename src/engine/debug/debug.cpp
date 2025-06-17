#include <engine/debug/debug.h>

#include <engine/graphics/window.h>
#include <engine/graphics/renderer.h>
#include <engine/file/resource_manager.h>

namespace engine {

	static void draw_text_right_aligned() {
		// TODO:
		// take in a string and a rect, draw string right aligned in that rect
	}

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->test_screens.rendering.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input, Window* window) {
		/* Update test screens */
		debug->test_screens.rendering.update(input);

		/* Show CPU profiling information */
		window->set_title(std::format("{} ({:.1f} fps)", "Game", 1.0f / debug->performance.frame_timer.average_delta())); // FIXME: We should replace "Game" with some constant
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution) {
		/* Draw test screens */
		debug.test_screens.rendering.draw(renderer, screen_resolution);

		/* Render CPU profiling overlay */
		FontID debug_font_id = FontID(1); // FIXME: we should store this in DebugState
		int32_t font_size = 16;

		// TODO: render text right aligned
		std::string text = std::format("render: {:.1f} ms", debug.performance.render_timer.average_delta() * 1e3);
		int32_t text_width = resources->font(debug_font_id).text_width(font_size, text);
		renderer->draw_text(debug_font_id, font_size, IVec2 { screen_resolution.x - text_width, font_size }, RGBA::white(), text);

		// TODO: print input frame delta
		// TODO: print update frame delta
		// TODO: print total frame delta
	}

} // namespace engine
