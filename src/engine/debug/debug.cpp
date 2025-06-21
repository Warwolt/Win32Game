#include <engine/debug/debug.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

namespace engine {

	constexpr int32_t DEBUG_UI_FONT_SIZE = 16;

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

	static void draw_cpu_timing_overlay(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution) {
		auto draw_section_timing = [&](const std::string& label, const DeltaTimer& timer, int32_t y) {
			std::string text = std::format("{}: {:.2f} ms", label, timer.average_delta() * 1e3);
			draw_text_right_aligned(renderer, resources, debug.debug_font_id, DEBUG_UI_FONT_SIZE, { 0, DEBUG_UI_FONT_SIZE * y }, screen_resolution.x, RGBA::white(), text);
		};
		draw_section_timing("input", debug.performance.input_timer, 1);
		draw_section_timing("update", debug.performance.update_timer, 2);
		draw_section_timing("draw", debug.performance.draw_timer, 3);
		draw_section_timing("render", debug.performance.render_timer, 4);
		draw_section_timing("frame", debug.performance.frame_timer, 5);
		std::string text = std::format("fps: {:>4}   ", (int)(1.0f / debug.performance.frame_timer.average_delta()));
		draw_text_right_aligned(renderer, resources, debug.debug_font_id, DEBUG_UI_FONT_SIZE, { 0, DEBUG_UI_FONT_SIZE * 6 }, screen_resolution.x, RGBA::white(), text);
	}

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->debug_font_id = resources->load_font("assets/font/ModernDOS8x16.ttf").value();
		debug->rendering_test_screen.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input, Window* window) {
		/* Update test screens */
		if (debug->show_rendering_test_screen) {
			debug->rendering_test_screen.update(input);
		}

		/* Show CPU profiling information in window title */
		window->set_title(std::format("{} ({:.1f} fps)", "Game", 1.0f / debug->performance.frame_timer.average_delta())); // FIXME: We should replace "Game" with some constant

		if (input.keyboard.key_was_pressed_now(VK_F3)) {
			debug->show_cpu_timing_overlay = !debug->show_cpu_timing_overlay;
		}

		if (input.keyboard.key_was_pressed_now(VK_MENU)) {
			debug->menu_bar_focused = !debug->menu_bar_focused;
		}
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution) {
		/* Draw test screens */
		if (debug.show_rendering_test_screen) {
			debug.rendering_test_screen.draw(renderer, debug.debug_font_id, screen_resolution);
		}

		constexpr RGBA menu_bar_color_bg = { 177, 177, 177, 255 };
		constexpr RGBA menu_bar_color_focus = { 197, 197, 197, 255 };
		constexpr RGBA menu_bar_color_text = { 0, 0, 0, 255 };
		constexpr int32_t menu_bar_height = DEBUG_UI_FONT_SIZE + 4;
		constexpr int32_t menu_item_padding = 8;
		constexpr int32_t menu_item_focus_padding = 2;

		/* Draw menu bar */
		renderer->draw_rect_fill(Rect { 0, 0, screen_resolution.x, menu_bar_height }, menu_bar_color_bg);
		if (debug.menu_bar_focused) {
			// draw focus
			int32_t text_width = resources->font(debug.debug_font_id).text_width(DEBUG_UI_FONT_SIZE, "Debug");
			int32_t focus_padding = menu_item_focus_padding;
			renderer->draw_rect_fill(Rect { menu_item_padding - focus_padding, focus_padding, text_width + focus_padding, DEBUG_UI_FONT_SIZE }, menu_bar_color_focus);

			// draw underline
			int32_t letter_width = resources->font(debug.debug_font_id).glyph(DEBUG_UI_FONT_SIZE, 'D').width;
			renderer->draw_line(IVec2 { menu_item_padding, DEBUG_UI_FONT_SIZE - 1 }, IVec2 { menu_item_padding + letter_width, DEBUG_UI_FONT_SIZE - 1 }, menu_bar_color_text);
		}
		renderer->draw_text(debug.debug_font_id, DEBUG_UI_FONT_SIZE, { 8, DEBUG_UI_FONT_SIZE - 2 }, menu_bar_color_text, "Debug");

		/* Render CPU profiling overlay */
		if (debug.show_cpu_timing_overlay) {
			draw_cpu_timing_overlay(renderer, debug, resources, screen_resolution);
		}
	}

} // namespace engine
