#include <engine/debug/debug.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>

namespace engine {

	static void draw_text_right_aligned(
		Renderer* renderer,
		ResourceManager* resources,
		FontID font_id,
		int32_t font_size,
		IVec2 pos,
		int32_t box_width,
		RGBA color,
		const std::string& text
	) {
		int32_t text_width = resources->font(font_id).text_width(font_size, text);
		renderer->draw_text(font_id, font_size, IVec2 { pos.x + box_width - text_width, pos.y }, color, text);
	}

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->debug_font_id = resources->load_font("assets/font/ModernDOS8x16.ttf").value();
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
		debug.test_screens.rendering.draw(renderer, debug.debug_font_id, screen_resolution);

		/* Render CPU profiling overlay */
		FontID debug_font_id = FontID(1); // FIXME: we should store this in DebugState
		auto draw_section_timing = [&](const std::string& label, const DeltaTimer& timer, int32_t y) {
			int32_t font_size = 16;
			std::string text = std::format("{}: {:.1f} ms", label, timer.average_delta() * 1e3);
			draw_text_right_aligned(renderer, resources, debug_font_id, font_size, { 0, font_size * y }, screen_resolution.x, RGBA::white(), text);
		};
		draw_section_timing("input", debug.performance.input_timer, 1);
		draw_section_timing("update", debug.performance.update_timer, 2);
		draw_section_timing("render", debug.performance.render_timer, 3);
	}

} // namespace engine
