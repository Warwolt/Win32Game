#include <engine/debug/assert.h>
#include <engine/engine.h>
#include <engine/input/input.h>
#include <engine/math/ivec2.h>
#include <game/game.h>

#include <expected>
#include <format>
#include <windows.h>
#include <windowsx.h>

// for prototyping only
#include <stb_truetype/stb_truetype.h>

struct ProgramContext {
	engine::EngineState engine;
	game::GameState game;
};

static ProgramContext g_context;

static engine::EngineState initialize_engine_or_abort(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
	std::expected<engine::EngineState, engine::EngineError> engine_result = engine::initialize(instance, wnd_proc, window_title);
	if (!engine_result) {
		if (auto* window_error = std::get_if<engine::WindowError>(&engine_result.error())) {
			std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(*window_error));
			MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
	return engine_result.value();
}

static void pump_window_messages() {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			g_context.engine.should_quit = true;
		}
	}
}

static void update_input() {
	engine::update_input_devices(&g_context.engine.input, g_context.engine.events, g_context.engine.window);
	g_context.engine.events = {};
}

static LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	switch (message) {
		case WM_SIZE: {
			engine::IVec2 window_size = g_context.engine.window.on_resized();
		} break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.engine.input.keyboard.on_key_event(key_id, true);
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.engine.input.keyboard.on_key_event(key_id, false);
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_MOUSEWHEEL: {
			g_context.engine.events.mouse.mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
		} break;

		case WM_MOUSEMOVE: {
			g_context.engine.events.mouse.mouse_x = (int16_t)GET_X_LPARAM(l_param);
			g_context.engine.events.mouse.mouse_y = (int16_t)GET_Y_LPARAM(l_param);
		} break;

		case WM_ACTIVATE: {
			g_context.engine.window.on_focus_changed(LOWORD(w_param) != WA_INACTIVE);
		} break;

		case WM_PAINT: {
			/* Input */
			update_input();

			/* Update */
			game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
			engine::update(&g_context.engine, g_context.engine.input);

			/* Render*/
			game::draw(&g_context.engine.renderer, g_context.game);
			engine::draw(&g_context.engine.renderer, g_context.engine);
			g_context.engine.renderer.render(&g_context.engine.bitmap, g_context.engine.resources);
			g_context.engine.window.render_wm_paint(g_context.engine.bitmap);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	/* Initialize */
	g_context.engine = initialize_engine_or_abort(instance, on_window_event, "Game");
	g_context.game = game::initialize(&g_context.engine);
	LOG_INFO("Initialized");

	/* Load font */
	// open file
	const char* font_path = "assets/font/dos437.ttf";
	FILE* font_file = fopen(font_path, "rb");
	DEBUG_ASSERT(font_file, "Couldn't open file %s", font_path);
	// get file size
	fseek(font_file, 0, SEEK_END);
	size_t font_filesize = ftell(font_file);
	fseek(font_file, 0, SEEK_SET);
	// read file contents
	uint8_t* font_buffer = (uint8_t*)malloc(font_filesize);
	fread(font_buffer, font_filesize, 1, font_file);
	// close file
	fclose(font_file);

	/* Prepare font */
	stbtt_fontinfo font_info;
	bool font_init_result = stbtt_InitFont(&font_info, font_buffer, 0);
	DEBUG_ASSERT(font_init_result, "Couldn't initialize font");

	/* Calculate font scaling */
	float font_height = 16.0f;
	float font_scale = stbtt_ScaleForPixelHeight(&font_info, font_height);
	int font_ascent, font_descent, font_line_gap;
	stbtt_GetFontVMetrics(&font_info, &font_ascent, &font_descent, &font_line_gap);
	font_ascent = (int)std::round(font_ascent * font_scale);
	font_descent = (int)std::round(font_descent * font_scale);

	/* Main loop */
	while (!g_context.engine.should_quit) {
		/* Input */
		pump_window_messages();
		update_input();

		/* Update */
		game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
		engine::update(&g_context.engine, g_context.engine.input);

		/* Render */
		game::draw(&g_context.engine.renderer, g_context.game);
		engine::draw(&g_context.engine.renderer, g_context.engine);
		g_context.engine.renderer.render(&g_context.engine.bitmap, g_context.engine.resources);

		// draw text
		int text_pos_x = 0;
		int text_pos_y = (int)font_height;
		for (char character : "the quick brown fox jumps over the lazy dog") {
			/* Character bounding box */
			int advance_width;
			int left_side_bearing;
			stbtt_GetCodepointHMetrics(&font_info, character, &advance_width, &left_side_bearing);

			/* Render character */
			// TODO: we should cache the bitmaps for the given character and size
			int width, height, xoff, yoff;
			uint8_t* character_bitmap = stbtt_GetCodepointBitmap(&font_info, font_scale, font_scale, character, &width, &height, &xoff, &yoff);
			for (int32_t y = 0; y < height; y++) {
				for (int32_t x = 0; x < width; x++) {
					engine::Pixel pixel = engine::Pixel::from_rgb(engine::RGBA::white());
					float alpha = character_bitmap[x + y * width] / 255.0f;
					g_context.engine.bitmap.put(text_pos_x + x + xoff, text_pos_y + y + yoff, pixel, alpha);
				}
			}
			stbtt_FreeBitmap(character_bitmap, nullptr);

			/* Advance x */
			text_pos_x += (int)std::round(advance_width * font_scale);
		}

		g_context.engine.window.render(g_context.engine.bitmap);
	}

	free(font_buffer);
	g_context.engine.resources.free_resources();

	LOG_INFO("Shutting down");
	return 0;
}
