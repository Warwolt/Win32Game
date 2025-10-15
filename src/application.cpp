#include <application.h>

#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <game/game.h>

#include <windows.h>
#include <windowsx.h>

#include <format>

#ifdef TRACY_ENABLE
constexpr bool PROFILING_IS_ENABLED = true;
#else
constexpr bool PROFILING_IS_ENABLED = false;
#endif

struct State {
	engine::Engine engine;
	game::Game game;
};

static void pump_window_messages(State* state) {
	CPUProfilingScope();
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			state->engine.should_quit = true;
		}
	}
}

static void update_input_devices(State* state) {
	CPUProfilingScope();
	engine::update_input_devices(&state->engine.input, state->engine.input_events, state->engine.window);
	state->engine.input_events = {};

	// HACK: we're not forwarding key events to DefWindowProc, so we have to handle ALT+F4 manually
	const engine::Keyboard& keyboard = state->engine.input.keyboard;
	if (keyboard.key_is_pressed(VK_MENU) && keyboard.key_was_pressed_now(VK_F4)) {
		state->engine.should_quit = true;
	}
}

State* initialize_application(HINSTANCE instance, WNDPROC on_window_event) {
	State* state = new State {};
	std::expected<engine::Engine, engine::EngineError> engine_result = engine::initialize(instance, on_window_event);
	if (!engine_result) {
		if (auto* window_error = std::get_if<engine::WindowError>(&engine_result.error())) {
			std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(*window_error));
			MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
	state->engine = std::move(engine_result.value());
	state->game = game::initialize(&state->engine);
	LOG_INFO("Initialized");
	if (PROFILING_IS_ENABLED) {
		LOG_INFO("CPU profiling enabled");
	}
	else {
		LOG_INFO("CPU profiling disabled");
	}
	return state;
}

LRESULT CALLBACK on_window_event(
	State* state,
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	CPUProfilingScope();
	switch (message) {
		case WM_SIZE: {
			engine::IVec2 window_size = state->engine.window.on_resized();
		} break;

		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			state->engine.input.keyboard.on_key_event(key_id, true);
			return (LRESULT)0;
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			state->engine.input.keyboard.on_key_event(key_id, false);
			return (LRESULT)0;
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
			state->engine.input_events.mouse.mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
		} break;

		case WM_MOUSEMOVE: {
			state->engine.input_events.mouse.mouse_x = (int16_t)GET_X_LPARAM(l_param);
			state->engine.input_events.mouse.mouse_y = (int16_t)GET_Y_LPARAM(l_param);
		} break;

		case WM_ACTIVATE: {
			state->engine.window.on_focus_changed(LOWORD(w_param) != WA_INACTIVE);
		} break;

		case WM_PAINT: {
			/* Input */
			update_input_devices(state);

			/* Update */
			game::update(&state->game, &state->engine.commands, state->engine.input);
			engine::update(&state->engine);

			/* Render*/
			game::draw(&state->engine.renderer, state->game);
			engine::draw(&state->engine);
			state->engine.renderer.render(&state->engine.bitmap, &state->engine.resources);
			state->engine.window.render_wm_paint(state->engine.bitmap);
		} break;
	}
	return DefWindowProc(window, message, w_param, l_param);
}

bool update_application(State* state) {
	state->engine.debug.frame_timer.start();
	{
		/* Input */
		pump_window_messages(state);
		update_input_devices(state);

		/* Update */
		game::update(&state->game, &state->engine.commands, state->engine.input);
		engine::update(&state->engine);

		/* Draw */
		game::draw(&state->engine.renderer, state->game);
		engine::draw(&state->engine);

		/* Render */
		state->engine.renderer.render(&state->engine.bitmap, &state->engine.resources);
		state->engine.window.render(state->engine.bitmap);
	}
	state->engine.debug.frame_timer.end();
	CPUProfilingEndFrame();

	return state->engine.should_quit;
}

void shutdown_application(State* state) {
	LOG_INFO("Shutting down");
	delete state;
}
