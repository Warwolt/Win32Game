#include <application.h>

#include <engine/debug/logging.h>

#include <windows.h>

#include <format>

namespace application {

	State initialize(HINSTANCE instance, WNDPROC on_window_event) {
		/* Initialize engine */
		engine::EngineResult engine_result = engine::initialize(instance, on_window_event);
		if (!engine_result) {
			if (auto* window_error = std::get_if<engine::WindowError>(&engine_result.error())) {
				std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(*window_error));
				MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
				exit(1);
			}
		}
		engine::Engine engine = std::move(engine_result.value());

		/* Initialize game */
		game::Game game = game::initialize(&engine);

		return {
			.engine = std::move(engine),
			.game = game,
		};
	}

	LRESULT CALLBACK on_window_event(
		State* state,
		HWND window,
		UINT message,
		WPARAM w_param,
		LPARAM l_param
	) {
		return DefWindowProc(window, message, w_param, l_param);
	}

	bool update(State* state) {
		LOG_INFO("Hello world!");
		return true;
	}

} // namespace application
