#include <engine/debug/assert.h>
#include <engine/engine.h>
#include <engine/input/input.h>
#include <engine/math/ivec2.h>
#include <game/game.h>

#include <expected>
#include <format>
#include <windows.h>
#include <windowsx.h>

// prototyping only
#include <engine/graphics/font.h>
#include <engine/graphics/font_id.h>
#include <utility>

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
	LPSTR /*command_linse*/,
	int /*command_show*/
) {
	/* Initialize */
	g_context.engine = initialize_engine_or_abort(instance, on_window_event, "Game");
	g_context.game = game::initialize(&g_context.engine);
	LOG_INFO("Initialized");

	engine::FontID font_id = g_context.engine.resources.load_font("assets/font/dos437.ttf").value();

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
		int font_size = 16;
		int text_pos_x = 0;
		int text_pos_y = font_size;
		engine::Typeface& typeface = *g_context.engine.resources.font(font_id);
		for (char character : "the quick brown fox jumps over the lazy dog") {
			const engine::Glyph& glyph = typeface.glyph(16, character);

			/* Render character */
			for (int32_t y = 0; y < glyph.height; y++) {
				for (int32_t x = 0; x < glyph.width; x++) {
					engine::Pixel pixel = engine::Pixel::from_rgb(engine::RGBA::white());
					float alpha = glyph.pixels[x + y * glyph.width] / 255.0f;
					g_context.engine.bitmap.put(text_pos_x + x, text_pos_y + y + glyph.y_offset, pixel, alpha);
				}
			}

			/* Advance x */
			text_pos_x += glyph.advance_width;
		}

		g_context.engine.window.render(g_context.engine.bitmap);
	}

	LOG_INFO("Shutting down");
	return 0;
}
