#include <engine/debug/assert.h>
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
			if (g_context.initialized) {
				/* Input */
				update_input();

				/* Update */
				game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
				engine::update(&g_context.engine, g_context.engine.input);

				/* Render*/
				game::draw(&g_context.engine.renderer, g_context.game);
				engine::draw(&g_context.engine.renderer, g_context.engine);
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
	const char* window_title = "Game";
	g_context.engine = initialize_engine_or_abort(instance, on_window_event, window_title);
	g_context.game = game::initialize(&g_context.engine);
	g_context.initialized = true;
	LOG_INFO("Initialized");

	int index = 0;
	int samples = 100;
	std::vector<float> frame_deltas;

	/* Main loop */
	while (!g_context.engine.should_quit) {
		const auto start = std::chrono::high_resolution_clock::now();

		/* Input */
		pump_window_messages();
		update_input();

		/* Update */
		game::update(&g_context.game, &g_context.engine.commands, g_context.engine.input);
		engine::update(&g_context.engine, g_context.engine.input);

		/* Render */
		game::draw(&g_context.engine.renderer, g_context.game);
		engine::draw(&g_context.engine.renderer, g_context.engine);
		g_context.engine.renderer.render(&g_context.engine.bitmap, &g_context.engine.resources);
		g_context.engine.window.render(g_context.engine.bitmap);

		// update FPS
		float frame_delta = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
		if (index++ < samples) {
			frame_deltas.push_back(frame_delta);
		}
		else {
			frame_deltas[index++ % samples] = frame_delta;
		}
		float avg_frame_delta = std::accumulate(frame_deltas.begin(), frame_deltas.end(), 0.0f) / frame_deltas.size();
		float avg_fps = 1.0f / avg_frame_delta;
		std::string title_with_fps = std::format("{} ({:.2f} fps)", window_title, avg_fps);
		g_context.engine.window.set_title(title_with_fps);
	}

	LOG_INFO("Shutting down");
	return 0;
}
