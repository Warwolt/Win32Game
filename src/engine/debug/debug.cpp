#include <engine/debug/assert.h>
#include <engine/debug/debug.h>
#include <engine/debug/logging.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

namespace engine {

	constexpr int32_t DEBUG_UI_FONT_SIZE = 16;

	struct Style {
		int32_t margin = 0;
		int32_t margin_top = 0;
		int32_t margin_right = 0;
		int32_t margin_bottom = 0;
		int32_t margin_left = 0;

		int32_t border = 0;
		int32_t border_top = 0;
		int32_t border_right = 0;
		int32_t border_bottom = 0;
		int32_t border_left = 0;

		int32_t padding = 0;
		int32_t padding_top = 0;
		int32_t padding_right = 0;
		int32_t padding_bottom = 0;
		int32_t padding_left = 0;

		FontID font_id;
		int32_t font_size;
		RGBA color;
		RGBA background_color = { 0 };
		RGBA border_color = { 0 };
	};

	static void draw_text_styled(Renderer* renderer, ResourceManager* resources, IVec2 position, const Style& style, const std::string& text) {
		DEBUG_ASSERT(style.font_id.value != 0, "Can't render font id 0!");
		DEBUG_ASSERT(style.font_size > 0, "Can't render font size 0!");

		/* Margin */
		int32_t margin_top = style.margin ? style.margin : style.margin_top;
		// int32_t margin_right = style.margin ? style.margin : style.margin_right;
		// int32_t margin_bottom = style.margin ? style.margin : style.margin_bottom;
		int32_t margin_left = style.margin ? style.margin : style.margin_left;

		/* Border */
		int32_t border_top = style.border ? style.border : style.border_top;
		int32_t border_right = style.border ? style.border : style.border_right;
		int32_t border_bottom = style.border ? style.border : style.border_bottom;
		int32_t border_left = style.border ? style.border : style.border_left;

		/* Padding */
		int32_t padding_top = style.padding ? style.padding : style.padding_top;
		int32_t padding_right = style.padding ? style.padding : style.padding_right;
		int32_t padding_bottom = style.padding ? style.padding : style.padding_bottom;
		int32_t padding_left = style.padding ? style.padding : style.padding_left;

		/* Widths and heights */
		Font& font = resources->font(style.font_id);
		int32_t content_width = font.text_width(style.font_size, text);
		int32_t content_height = style.font_size;

		int32_t padding_width = content_width + padding_left + padding_right;
		int32_t padding_height = content_height + padding_top + padding_bottom;

		int32_t border_width = padding_width + border_left + border_right;
		int32_t border_height = padding_height + border_top + border_bottom;

		/* Positions */
		IVec2 border_position = position + IVec2 { margin_left, margin_top };
		IVec2 padding_position = border_position + IVec2 { border_left, border_top };
		IVec2 content_position = padding_position + IVec2 { padding_left, padding_top };

		/* Draw background */
		if (style.background_color) {
			Rect rect = { padding_position.x, padding_position.y, padding_width, padding_height };
			renderer->draw_rect_fill(rect, style.background_color);
		}

		/* Draw border */
		if (style.border_color) {
			if (style.border == 1) {
				Rect rect = { border_position.x, border_position.y, border_width, border_height };
				renderer->draw_rect(rect, style.border_color);
			}
			else {
				Rect top = { border_position.x, border_position.y, border_width, border_top };
				Rect bottom = { border_position.x, border_position.y + border_top + padding_height, border_width, border_top };
				Rect left = { border_position.x, border_position.y + border_top, border_left, padding_height };
				Rect right = { border_position.x + border_left + padding_width, border_position.y + border_top, border_right, padding_height };
				renderer->draw_rect_fill(top, style.border_color);
				renderer->draw_rect_fill(bottom, style.border_color);
				renderer->draw_rect_fill(left, style.border_color);
				renderer->draw_rect_fill(right, style.border_color);
			}
		}

		/* Draw text */
		renderer->draw_text(style.font_id, style.font_size, content_position, style.color, text);
	}

	static void draw_text_right_aligned(Renderer* renderer, ResourceManager* resources, FontID font_id, int32_t font_size, IVec2 pos, int32_t box_width, RGBA color, const std::string& text) {
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
		std::string window_title_with_fps =
			std::format("{} ({:.1f} fps)", window_title, avg_fps);
		commands->push_back(AppCommand_SetWindowTitle { window_title_with_fps });

		/* Update Debug UI */
		{
			if (input.keyboard.key_was_pressed_now(VK_F3)) {
				debug->show_cpu_timing_overlay = !debug->show_cpu_timing_overlay;
			}

			/* Menu bar */
			if (!debug->menu_bar_active) {
				if (input.keyboard.key_was_pressed_now(VK_MENU)) {
					/* Open menu bar */
					debug->menu_bar_active = true;
					debug->menu_bar_show_shortcuts = true;
					debug->menu_bar_file_item_focused = true;
				}
			}
			else if (debug->menu_bar_active) {
				/* Close menu bar */
				if (input.keyboard.key_was_pressed_now(VK_MENU) ||
					input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
					debug->menu_bar_active = false;
					debug->menu_bar_show_shortcuts = false;
					debug->menu_bar_file_item_focused = false;
					debug->file_menu_open = false;
				}

				/* Open file menu */
				if (input.keyboard.key_was_pressed_now(VK_RETURN) ||
					input.keyboard.key_was_pressed_now('F')) {
					debug->menu_bar_file_item_focused = false;
					debug->file_menu_open = true;
					debug->file_menu_exit_item_focused = true;
				}

				/* File menu */
				if (debug->file_menu_open) {
					if (input.keyboard.key_was_pressed_now('X')) {
						commands->push_back(AppCommand_Quit());
					}
				}
			}
		}
	}

	void draw_debug(Renderer* renderer, const DebugState& debug, ResourceManager* resources, IVec2 screen_resolution) {
		/* Draw test screens */
		if (debug.show_rendering_test_screen) {
			debug.rendering_test_screen.draw(renderer, debug.debug_font_id, screen_resolution);
		}

		// test out stylized drawing
		Style test_style = {
			.margin = 0,
			.border = 10,
			.padding = 10,
			.font_id = debug.debug_font_id,
			.font_size = DEBUG_UI_FONT_SIZE,
			.color = RGBA::white(),
			.background_color = RGBA::red(),
			.border_color = RGBA::green(),
		};
		draw_text_styled(renderer, resources, { 0, 0 }, test_style, "The quick fox jumps over the lazy dog");

		/* Draw Debug UI */
		{
			// constexpr RGBA menu_color = { 177, 177, 177, 255 };
			// constexpr int32_t menu_bar_height = DEBUG_UI_FONT_SIZE + 6;

			// Style menu_item_style {
			// 	.margin = 2,
			// 	.border = 2,
			// 	.padding = 1,
			// 	.font_id = debug.debug_font_id,
			// 	.font_size = DEBUG_UI_FONT_SIZE,
			// 	.color = { 0, 0, 0, 255 },
			// 	.background_color = debug.menu_bar_active ? RGBA { 191, 191, 191, 255 } : RGBA { 0 },
			// 	.border_color = debug.menu_bar_file_item_focused ? RGBA { 204, 204, 204, 255 } : RGBA { 0 },
			// };

			// /* Draw menu bar */
			// renderer->draw_rect_fill(Rect { 0, 0, screen_resolution.x, menu_bar_height }, menu_color);
			// draw_text_styled(renderer, resources, { 0, 0 }, menu_item_style, "File");
			// if (debug.menu_bar_show_shortcuts) {
			// 	// draw underscore
			// 	Style& style = menu_item_style;
			// 	Font& font = resources->font(debug.debug_font_id);
			// 	int32_t ascent = font.ascent(DEBUG_UI_FONT_SIZE);
			// 	int32_t letter_width = font.glyph(DEBUG_UI_FONT_SIZE, 'F').width;
			// 	IVec2 offset = { style.margin + style.padding,
			// 					 style.margin + style.padding + ascent };
			// 	renderer->draw_line(offset + IVec2 { 0, 1 }, offset + IVec2 { letter_width, 1 }, style.color);
			// }

			// /* File menu */
			// Style file_menu_item_style = menu_item_style;
			// file_menu_item_style.margin = 0;
			// file_menu_item_style.background_color =
			// 	debug.menu_bar_active ? RGBA { 191, 191, 191, 255 } : RGBA { 0 };
			// file_menu_item_style.border_color =
			// 	debug.file_menu_exit_item_focused ? RGBA { 204, 204, 204, 255 } : RGBA { 0 };

			// if (debug.file_menu_open) {
			// 	draw_text_styled(renderer, resources, { 0, menu_bar_height }, file_menu_item_style, "Exit");

			// Style& style = file_menu_item_style;
			// IVec2 offset = { style.margin + style.padding, style.margin +
			// style.padding }; IVec2 text_pos = offset + IVec2 { 8, menu_bar_height
			// }; renderer->draw_rect_fill(Rect { 0, menu_bar_height, 64,
			// DEBUG_UI_FONT_SIZE + offset.y }, menu_color);
			// renderer->draw_text(debug.debug_font_id, DEBUG_UI_FONT_SIZE, text_pos,
			// file_menu_item_style.color, "Exit");

			// // draw underscore
			// Font& font = resources->font(debug.debug_font_id);
			// int32_t ascent = font.ascent(DEBUG_UI_FONT_SIZE);
			// int32_t underscore_offset = font.text_width(DEBUG_UI_FONT_SIZE, "E");
			// int32_t letter_width = font.glyph(DEBUG_UI_FONT_SIZE, 'x').width;
			// renderer->draw_line(text_pos + IVec2 { underscore_offset, ascent + 1 },
			// text_pos + IVec2 { underscore_offset + letter_width, ascent + 1 },
			// style.color);
			// }
		}

		/* Render CPU profiling overlay */
		if (debug.show_cpu_timing_overlay) {
			draw_cpu_timing_overlay(renderer, debug, resources, screen_resolution);
		}
	}

} // namespace engine
