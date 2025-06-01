#include <engine/debug/logging.h>
#include <engine/engine.h>
#include <engine/input/gamepad.h>
#include <engine/input/input.h>
#include <engine/math/ivec2.h>
#include <game/game.h>

#include <format>
#include <windows.h>
#include <windowsx.h>

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
	engine::update_input_devices(&g_context.engine.input, g_context.engine.events);
	g_context.engine.input.window_size = {
		.x = g_context.engine.window.bitmap.width,
		.y = g_context.engine.window.bitmap.height
	};
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
			if (window == g_context.engine.window.handle) {
				engine::on_window_resized(&g_context.engine.window);
			}
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

		case WM_PAINT: {
			/* Input */
			update_input();

			/* Update */
			game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
			engine::update(&g_context.engine, g_context.engine.input);

			/* Render*/
			game::draw(&g_context.engine.renderer, g_context.game);
			engine::draw(&g_context.engine.renderer, g_context.engine);

			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			g_context.engine.renderer.render(&g_context.engine.window, device_context);
			EndPaint(window, &paint);
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

	/* Main loop */
	while (!g_context.engine.should_quit) {
		/* Input */
		pump_window_messages();
		update_input();

		/* Update */
		game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
		engine::update(&g_context.engine, g_context.engine.input);

		/* Render */
		g_context.engine.renderer.clear_screen();
		game::draw(&g_context.engine.renderer, g_context.game);
		engine::draw(&g_context.engine.renderer, g_context.engine);

		HDC device_context = GetDC(g_context.engine.window.handle);
		g_context.engine.renderer.render(&g_context.engine.window, device_context);
		ReleaseDC(g_context.engine.window.handle, device_context);
	}

	return 0;
}
