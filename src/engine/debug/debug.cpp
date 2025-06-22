#include <engine/debug/debug.h>

#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

#include <engine/debug/logging.h>

namespace engine {

	constexpr int32_t DEBUG_UI_FONT_SIZE = 16;

	struct Style {
		int32_t padding;
		int32_t margin;
		FontID font_id;
		int32_t font_size;
		RGBA color;
		RGBA background_color = { 0, 0, 0, 0 };
	};

	static void draw_text_styled(
		Renderer* renderer,
		ResourceManager* resources,
		IVec2 position,
		const Style& style,
		const std::string& text
	) {
		IVec2 margined_pos = position + IVec2 { style.margin, style.margin };
		IVec2 padded_position = margined_pos + IVec2 { style.padding, style.padding };
		if (style.background_color) {
			Font& font = resources->font(style.font_id);
			int32_t text_width = font.text_width(style.font_size, text);
			renderer->draw_rect_fill(Rect { margined_pos.x, margined_pos.y, text_width + 2 * style.padding, style.font_size + 2 * style.padding }, style.background_color);
		}
		renderer->draw_text(style.font_id, style.font_size, padded_position, style.color, text);
	}

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
		draw_section_timing("input", debug.performance.input_timer, 0);
		draw_section_timing("update", debug.performance.update_timer, 1);
		draw_section_timing("draw", debug.performance.draw_timer, 2);
		draw_section_timing("render", debug.performance.render_timer, 3);
		draw_section_timing("frame", debug.performance.frame_timer, 4);
		std::string text = std::format("fps: {:>4}   ", (int)(1.0f / debug.performance.frame_timer.average_delta()));
		draw_text_right_aligned(renderer, resources, debug.debug_font_id, DEBUG_UI_FONT_SIZE, { 0, DEBUG_UI_FONT_SIZE * 5 }, screen_resolution.x, RGBA::white(), text);
	}

	void initialize_debug(DebugState* debug, ResourceManager* resources) {
		debug->debug_font_id = resources->load_font("assets/font/ModernDOS8x16.ttf").value();
		debug->rendering_test_screen.initialize(resources);
	}

	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands) {
		/* Update test screens */
		if (debug->show_rendering_test_screen) {
			debug->rendering_test_screen.update(input);
		}

		/* Show CPU profiling information in window title */
		float avg_fps = 1.0f / debug->performance.frame_timer.average_delta();
		const char* window_title = "Game";
		std::string window_title_with_fps = std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->push_back(AppCommand_SetWindowTitle { window_title_with_fps });

		/* Update Debug UI */
		{
			if (input.keyboard.key_was_pressed_now(VK_F3)) {
				debug->show_cpu_timing_overlay = !debug->show_cpu_timing_overlay;
			}

			if (input.keyboard.key_was_pressed_now(VK_MENU)) {
				debug->menu_bar_focused = !debug->menu_bar_focused;
				if (!debug->menu_bar_focused) {
					debug->file_menu_opened = false;
				}
			}

			if (debug->menu_bar_focused) {
				if (input.keyboard.key_was_pressed_now('F')) {
					debug->file_menu_opened = true;
				}

				if (debug->file_menu_opened) {
					if (input.keyboard.key_was_pressed_now('X')) {
						commands->push_back(AppCommand_Quit());
					}
				}

				if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
					debug->menu_bar_focused = false;
					debug->file_menu_opened = false;
				}
			}
		}
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution) {
		/* Draw test screens */
		if (debug.show_rendering_test_screen) {
			debug.rendering_test_screen.draw(renderer, debug.debug_font_id, screen_resolution);
		}

		/* Draw Debug UI */
		{
			constexpr RGBA menu_color = { 177, 177, 177, 255 };
			constexpr int32_t menu_bar_height = DEBUG_UI_FONT_SIZE + 6;

			Style menu_item_style {
				.padding = 1,
				.margin = 2,
				.font_id = debug.debug_font_id,
				.font_size = DEBUG_UI_FONT_SIZE,
				.color = { 0, 0, 0, 255 },
				.background_color = debug.menu_bar_focused ? RGBA { 199, 199, 199, 255 } : RGBA { 0 },
			};

			/* Draw menu bar */
			renderer->draw_rect_fill(Rect { 0, 0, screen_resolution.x, menu_bar_height }, menu_color);
			draw_text_styled(renderer, resources, { 0, 0 }, menu_item_style, "File");
			if (debug.menu_bar_focused) {
				// draw underscore
				Style& style = menu_item_style;
				Font& font = resources->font(debug.debug_font_id);
				int32_t ascent = font.ascent(DEBUG_UI_FONT_SIZE);
				int32_t letter_width = font.glyph(DEBUG_UI_FONT_SIZE, 'F').width;
				IVec2 offset = { style.margin + style.padding, style.margin + style.padding + ascent };
				renderer->draw_line(offset + IVec2 { 0, 1 }, offset + IVec2 { letter_width, 1 }, style.color);
			}

			/* File menu */
			if (debug.file_menu_opened) {
				Style& style = menu_item_style;
				IVec2 offset = { style.margin + style.padding, style.margin + style.padding };
				IVec2 text_pos = offset + IVec2 { 8, menu_bar_height };
				renderer->draw_rect_fill(Rect { 0, menu_bar_height, 64, DEBUG_UI_FONT_SIZE + offset.y }, menu_color);
				renderer->draw_text(debug.debug_font_id, DEBUG_UI_FONT_SIZE, text_pos, menu_item_style.color, "Exit");

				// draw underscore
				Font& font = resources->font(debug.debug_font_id);
				int32_t ascent = font.ascent(DEBUG_UI_FONT_SIZE);
				int32_t underscore_offset = font.text_width(DEBUG_UI_FONT_SIZE, "E");
				int32_t letter_width = font.glyph(DEBUG_UI_FONT_SIZE, 'x').width;
				renderer->draw_line(text_pos + IVec2 { underscore_offset, ascent + 1 }, text_pos + IVec2 { underscore_offset + letter_width, ascent + 1 }, style.color);
			}
		}

		/* Render CPU profiling overlay */
		if (debug.show_cpu_timing_overlay) {
			draw_cpu_timing_overlay(renderer, debug, resources, screen_resolution);
		}
	}

} // namespace engine
