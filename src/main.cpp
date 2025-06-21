#include <engine/debug/assert.h>
#include <engine/debug/delta_timer.h>
#include <engine/engine.h>
#include <engine/input/input.h>
#include <engine/math/ivec2.h>
#include <game/game.h>

#include <windows.h>
#include <windowsx.h>

#include <array>
#include <chrono>
#include <expected>
#include <format>
#include <numeric>

struct ProgramContext {
	bool initialized = false;
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
	// HACK: we're not forwarding key events to DefWindowProc, so we have to handle ALT+F4 manually
	const engine::Keyboard& keyboard = g_context.engine.input.keyboard;
	if (keyboard.key_is_pressed(VK_MENU) && keyboard.key_was_pressed_now(VK_F4)) {
		g_context.engine.should_quit = true;
	}
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

		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.engine.input.keyboard.on_key_event(key_id, true);
			return (LRESULT)1;
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.engine.input.keyboard.on_key_event(key_id, false);
			return (LRESULT)1;
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
			if (g_context.initialized) {
				/* Input */
				update_input();

				/* Update */
				game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
				engine::update(&g_context.engine, g_context.engine.input);

				/* Render*/
				game::draw(&g_context.engine.renderer, g_context.game);
				engine::draw(&g_context.engine.renderer, &g_context.engine);
				g_context.engine.renderer.render(&g_context.engine.bitmap, &g_context.engine.resources);
				g_context.engine.window.render_wm_paint(g_context.engine.bitmap);
			}
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
	g_context.initialized = true;
	LOG_INFO("Initialized");

	/* Timing */
	engine::DeltaTimer& frame_timer = g_context.engine.debug.performance.frame_timer;
	engine::DeltaTimer& input_timer = g_context.engine.debug.performance.input_timer;
	engine::DeltaTimer& update_timer = g_context.engine.debug.performance.update_timer;
	engine::DeltaTimer& draw_timer = g_context.engine.debug.performance.draw_timer;
	engine::DeltaTimer& render_timer = g_context.engine.debug.performance.render_timer;

	/* Main loop */
	while (!g_context.engine.should_quit) {
		/* Start frame */
		frame_timer.start();

		/* Input */
		input_timer.start();
		pump_window_messages();
		update_input();
		input_timer.end();

		/* Update */
		update_timer.start();
		game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
		engine::update(&g_context.engine, g_context.engine.input);
		update_timer.end();

		/* Draw */
		draw_timer.start();
		game::draw(&g_context.engine.renderer, g_context.game);
		engine::draw(&g_context.engine.renderer, &g_context.engine);
		draw_timer.end();

		/* Render */
		render_timer.start();
		g_context.engine.renderer.render(&g_context.engine.bitmap, &g_context.engine.resources);
		g_context.engine.window.render(g_context.engine.bitmap);
		render_timer.end();

		/* End frame */
		frame_timer.end();
	}

	LOG_INFO("Shutting down");
	return 0;
}
